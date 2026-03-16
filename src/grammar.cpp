#include "../include/grammar.hpp"
#include "../include/lex.hpp"
#include "../include/rdesc.hpp"

#include "../rdesc/include/cst_macros.h"
#include "../rdesc/include/grammar.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>

using std::weak_ptr, std::shared_ptr;


shared_ptr<Grammar> global_grammar() {
    static weak_ptr<Grammar> global_grammar;

    if (global_grammar.expired()) {
        auto new_global_grammar = Grammar::create(
            NT_COUNT, NT_VARIANT_COUNT, NT_BODY_LENGTH,
            (const rdesc_grammar_symbol *)(grammar)
        );

        global_grammar = new_global_grammar;

        return shared_ptr(new_global_grammar);
    } else {
        return shared_ptr(global_grammar);
    }
}

void node_printer(const struct rdesc_node *n, FILE *out) {
    if (rtype(n) == RDESC_TOKEN) {
        fprintf(out, "[shape=record,label=\"");
        if (rid(n) == TK_IDENT) {
            IdentInfo *seminfo;
            memcpy(&seminfo, rseminfo(n), sizeof(void *));

            fprintf(out, "{{ident|%zu}}", seminfo->id);
        } else if (rid(n) == TK_NUM) {
            NumInfo *seminfo;
            memcpy(&seminfo, rseminfo(n), sizeof(void *));

            fprintf(out, "{{num|base: %d, %s}}",
                    seminfo->base, seminfo->num.c_str());
        } else {
            /* ignoring seminfo of table_value */
            fprintf(out, "%s", tk_names_escaped[rid(n)]);
        }
        fprintf(out, "\"]");
    } else {
        fprintf(out, "[label=\"%s\"]", nt_names[rid(n)]);
    }
}

void tk_destroyer(uint16_t id, void *seminfo_) {
    if (id == TK_NUM || id == TK_IDENT) {
        SemInfo *seminfo;
        memcpy(&seminfo, seminfo_, sizeof(void *));

        delete seminfo;
    }
}
