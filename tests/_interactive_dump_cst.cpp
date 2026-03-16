#include "../include/rdesc.hpp"
#include "../include/grammar.hpp"
#include "../include/lex.hpp"
#include "../src/detail.h"  // IWYU pragma: keep

#include <cstdint>
#include <string>

using std::cin;
using std::string;


int main() {
    auto parser = global_grammar()->new_parser();

    parser.start(NT_STMT);

    Lex lex { cin };

    uint16_t tk;
    rdesc_result res;

    do {
        tk = lex.next();
        void *current_seminfo = lex.get_current_seminfo();
        res = parser.pump(tk, &current_seminfo);
        assert(res!= RDESC_NOMATCH, "could not parse grammar");
    } while (res == RDESC_CONTINUE);

    assert(res == RDESC_READY, "could not complete CST");

    parser.dump_cst(stdout);
}
