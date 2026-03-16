/**
 * @file rdesc.hpp
 * @brief RAII wrapper for rdesc C library.
 */

#pragma once
#ifndef RDESC_HPP
#define RDESC_HPP


#include "grammar.hpp"

#include "../rdesc/include/grammar.h"
#include "../rdesc/include/rdesc.h"
#include "../src/detail.h"

#include "../rdesc/include/util.h"

#include <cstddef>
#include <cstdint>
#include <memory>

class Interpreter;


class Rdesc;

/** @brief `rdesc` context-free grammar. */
class Grammar : public std::enable_shared_from_this<Grammar> {
private:
    struct Token { explicit Token() = default; };

public:
    Grammar(Token, size_t nt_count, size_t nt_v_count, size_t nt_len,
        const rdesc_grammar_symbol *rules) {
        unwrap(rdesc_grammar_init(&grammar,
                                  nt_count, nt_v_count, nt_len, rules));
    }

    /** SAFETY: cannot copy `struct rdesc_grammar` safely */
    Grammar(const Grammar &) = delete;

    ~Grammar()
        { rdesc_grammar_destroy(&grammar); }

    auto operator*()
        { return &grammar; }

    Rdesc new_parser();

private:
    friend std::shared_ptr<Grammar> global_grammar();

    template<typename... Args>
    static auto create(Args... args)
        { return std::make_shared<Grammar>(Token {}, args...); }

    struct rdesc_grammar grammar;
};

/** @brief `struct rdesc` RAII control. */
class Rdesc {
public:
    Rdesc(std::shared_ptr<Grammar> grammar_)
        : grammar { grammar_ } {
        unwrap(rdesc_init(&p, **grammar_, sizeof(void *), tk_destroyer));
    }

    /** SAFETY: cannot copy `struct rdesc` safely */
    Rdesc(const Rdesc &) = delete;

    /** SAFETY: move constructor invalidates `struct rdesc` */
    Rdesc(Rdesc &&other) {
        p = other.p;
        grammar = other.grammar;
        other.destroyed = true;
    };

    ~Rdesc() {
        if (!destroyed) {
            rdesc_destroy(&p);
        }
    }

    void start(int start_symbol)
        { unwrap(rdesc_start(&p, start_symbol)); }

    void reset()
        { rdesc_reset(&p); }

    auto pump(uint16_t id, void *seminfo)
        { return rdesc_pump(&p, id, seminfo); }

    auto root()
        { return rdesc_root(&p); }

    void dump_cst(FILE *out)
        { rdesc_dump_cst(out, &p, node_printer); }

private:
    friend Interpreter;

    struct rdesc p;

    bool destroyed = false;

    std::shared_ptr<Grammar> grammar  /**< shared_ptr to Grammar class in just for
                                           preventing deletion of underlying
                                           struct rdesc_cfg. */;
};


inline Rdesc Grammar::new_parser()
    { return Rdesc(shared_from_this()); }


#endif
