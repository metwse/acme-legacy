#include "../include/interpreter.hpp"
#include "../include/core.hpp"
#include "../include/lex.hpp"

#include <iostream>
#include <string>
#include <utility>

using std::cin, std::cout;
using std::string;


int main() {
    auto parser = global_grammar()->new_parser();

    Lex lex { cin };

    Interpreter intr { std::move(parser) };

    enum rdesc_result res;
    while (true) {
        do {
            auto tk = lex.next();
            void *seminfo = lex.get_current_seminfo();

            res = intr.pump(tk, &seminfo);
        } while (res == RDESC_CONTINUE);

        if (res == RDESC_NOMATCH) {
            intr.dump(cout, lex);
            break;
        }
    };
}
