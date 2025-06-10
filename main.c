#include "nfa.h"
#include "re_compiler.h"
#include <stdio.h>
#include "ptrset.h"

int
main(void)
{
        struct re_compiler *rp = NULL;
        struct nfa *np = NULL;

        rp = re_compiler_new("abc", 6);

        np = re_compiler_comp(rp);

        nfa_dump(np);
        nfa_free(&np);
        re_compiler_free(&rp);
        (void)rp;
}
