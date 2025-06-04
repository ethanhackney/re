#include "comp.h"
#include "nfa.h"
#include <stddef.h>
#include <stdio.h>

/**
 * do regex compilation:
 *
 * args:
 *  @re: regex
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
static struct nfa *re_do_comp(const char **spp);

/**
 * compile or expression:
 *
 * args:
 *  @spp:     pointer to pointer to string
 *  @startpp: pointer to pointer to start nfa{}
 *  @endpp:   pointer to pointer to end nfa{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static void re_comp_or(const char **spp,
                       struct nfa **startpp,
                       struct nfa **endpp);

/**
 * compile regex factor:
 *
 * args:
 *  @spp:     pointer to pointer to string
 *  @startpp: pointer to pointer to start nfa{}
 *  @endpp:   pointer to pointer to end nfa{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static void re_comp_factor(const char **spp,
                           struct nfa **startpp,
                           struct nfa **endpp);

struct nfa *
re_comp(const char *re)
{
        return re_do_comp(&re);
}

static struct nfa *
re_do_comp(const char **spp)
{
        struct nfa *start2 = NULL;
        struct nfa *end2 = NULL;
        struct nfa *start = NULL;
        struct nfa *end = NULL;
        struct nfa *cur = NULL;

        /**
         *    +->CHAR-+
         *    |       |
         * OR-+       +->MATCH
         *    |       |
         *    +->CHAR-+
         */
        re_comp_or(spp, &start, &end);

        cur = start;
        while (**spp) {
                /**
                 *    +->CHAR-+
                 *    |       |
                 * OR-+       +->MATCH
                 *    |       |
                 *    +->CHAR-+
                 */
                re_comp_or(spp, &start2, &end2);

                /**
                 *    +->CHAR-+
                 *    |       |
                 * OR-+       +-x
                 *    |       |
                 *    +->CHAR-+
                 */
                nfa_free(&end);

                /**
                 *    +->CHAR-+     +->CHAR-+
                 *    |       |     |       |
                 * OR-+       +->OR-+       +->MATCH
                 *    |       |     |       |
                 *    +->CHAR-+     +->CHAR-+
                 */
                cur->n_edges[0] = start2;
                cur = start2;
                end = end2;
        }

        return start;
}


static void
re_comp_or(const char **spp, struct nfa **startpp, struct nfa **endpp)
{
        struct nfa *start2 = NULL;
        struct nfa *end2 = NULL;
        struct nfa *start1 = NULL;
        struct nfa *end1 = NULL;
        struct nfa *end = NULL;
        struct nfa *or = NULL;
        struct nfa *left = NULL;

        /**
         * CHAR->MATCH
         */
        re_comp_factor(spp, &start1, &end1);

        left = start1;
        end = end1;

        while (**spp == '|') {
                (*spp)++;

                /**
                 * CHAR->MATCH
                 */
                re_comp_factor(spp, &start2, &end2);

                /**
                 * CHAR-x
                 * CHAR-x
                 */
                nfa_free(&end);
                nfa_free(&end2);

                /**
                 *    +->CHAR-+
                 *    |       |
                 * OR-+       +->MATCH
                 *    |       |
                 *    +->CHAR-+
                 */
                or = nfa_or_new(left, start2);
                end = nfa_char_new(0);
                left->n_edges[0] = end;
                start2->n_edges[0] = end;

                left = or;
        }

        *startpp = left;
        *endpp = end;
}

static void
re_comp_factor(const char **spp, struct nfa **startpp, struct nfa **endpp)
{
        /**
         * CHAR->MATCH
         */
        *startpp = nfa_char_new(**spp);
        *endpp = nfa_char_new(0);
        (*startpp)->n_edges[0] = *endpp;
        (*spp)++;
}
