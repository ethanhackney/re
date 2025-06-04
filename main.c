#include "comp.h"
#include "nfa.h"
#include <stddef.h>

int
main(void)
{
        struct nfa *np = NULL;

        /**
         *          +->CHAR-+
         *          |       |
         * CHAR->OR-+       +->MATCH
         *          |       |
         *          +->CHAR-+
         */
        np = re_comp("ab|c|d");
        nfa_dump(np, 0);
        nfa_free(&np);
}
