#include "nfa.h"
#include "re_compiler.h"
#include <stdio.h>

int
main(void)
{
        struct re_compiler *rp = NULL;
        struct nfa *start = NULL;
        struct nfa *end = NULL;

        rp = re_compiler_new("[a-z]+", 6);

        end = nfa_epsilon_new();
        start = nfa_char_new(end, 'e');

        printf("%c\n", nfa_char(start));
        (void)rp;
}
