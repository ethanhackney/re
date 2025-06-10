#include "nfa.h"
#include "re_compiler.h"
#include <stdio.h>

int
main(void)
{
        struct re_compiler *rp = NULL;
        struct nfa *np = NULL;

        rp = re_compiler_new("a", 6);

        np = re_compiler_comp(rp);
        printf("%c\n", nfa_char(np));

        re_compiler_free(&rp);
        (void)rp;
}
