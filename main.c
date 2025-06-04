#include "comp.h"
#include "nfa.h"
#include <stddef.h>
#include <stdio.h>

int
main(void)
{
        struct re_compiler comp = {0};
        struct nfa *np = NULL;
        int finish = -1;

        re_compiler_init(&comp, "ab|c");

        /**
         *          +->CHAR-+
         *          |       |
         * CHAR->OR-+       +->MATCH
         *          |       |
         *          +->CHAR-+
         */
        np = re_compiler_comp(&comp);
        finish = re_compiler_nstates(&comp);
        printf("finishing state is %d\n", finish);

        nfa_dump(np, 0);
        nfa_free(&np);
        re_compiler_free(&comp);
}
