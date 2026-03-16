#include "../include/grammar.hpp"
#include "../src/detail.h"  // IWYU pragma: keep, false positive main():4

#include "../rdesc/include/grammar.h"
#include "../rdesc/include/util.h"


int main()
{
    struct rdesc_grammar g;

    unwrap(rdesc_grammar_init(&g, NT_COUNT, NT_VARIANT_COUNT, NT_BODY_LENGTH,
                              (struct rdesc_grammar_symbol *) grammar));

    rdesc_dump_bnf(stdout, &g, tk_names, nt_names);

    rdesc_grammar_destroy(&g);
}
