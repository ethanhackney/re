#include "nfa.h"
#include <stdio.h>

int
main(void)
{
        struct nfa *start = NULL;
        struct nfa *end = NULL;

        end = nfa_epsilon_new();
        start = nfa_char_new(end, 'e');

        nfa_free(&end);
        nfa_free(&start);
}
