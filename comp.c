#include "comp.h"
#include "nfa.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/**
 * do regex compilation:
 *
 * args:
 *  @spp:     pointer to pointer to string
 *  @startpp: pointer to pointer to start nfa{}
 *  @endpp:   pointer to pointer to end nfa{}
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
static void re_do_comp(const char **spp,
                       struct nfa **startpp,
                       struct nfa **endpp);

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
        struct nfa *start = NULL;
        struct nfa *end = NULL;

        re_do_comp(&re, &start, &end);

        return start;
}

static void
re_do_comp(const char **spp, struct nfa **startpp, struct nfa **endpp)
{
        struct nfa *start2 = NULL;
        struct nfa *end2 = NULL;
        struct nfa *start = NULL;
        struct nfa *end = NULL;
        struct nfa *cur = NULL;

        /**
         *    +->EXPR-+
         *    |       |
         * OR-+       +->MATCH
         *    |       |
         *    +->EXPR-+
         */
        re_comp_or(spp, &start, &end);

        cur = start;
        while (**spp) {
                /**
                 *    +->EXPR-+
                 *    |       |
                 * OR-+       +->MATCH
                 *    |       |
                 *    +->EXPR-+
                 */
                re_comp_or(spp, &start2, &end2);

                /**
                 *    +->EXPR-+
                 *    |       |
                 * OR-+       +-x
                 *    |       |
                 *    +->EXPR-+
                 */
                nfa_free(&end);

                /**
                 *    +->EXPR-+
                 *    |       |
                 * OR-+       +>MATCH
                 *    |       |
                 *    +->EXPR-+
                 */
                cur->n_edges[0] = start2;
                cur = start2;
                end = end2;
        }

        *startpp = start;
        *endpp = end;
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
                 *    +->EXPR-+
                 *    |       |
                 * OR-+       +->MATCH
                 *    |       |
                 *    +->EXPR-+
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
