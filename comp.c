#include "comp.h"
#include "nfa.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/**
 * do regex compilation:
 *
 * args:
 *  @rp:      pointer to re_compiler{}
 *  @startpp: pointer to pointer to start nfa{}
 *  @endpp:   pointer to pointer to end nfa{}
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
static void re_do_comp(struct re_compiler *rp,
                       struct nfa **startpp,
                       struct nfa **endpp);

/**
 * compile or expression:
 *
 * args:
 *  @rp:      pointer to re_compiler{}
 *  @startpp: pointer to pointer to start nfa{}
 *  @endpp:   pointer to pointer to end nfa{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static void re_comp_or(struct re_compiler *rp,
                       struct nfa **startpp,
                       struct nfa **endpp);

/**
 * compile regex factor:
 *
 * args:
 *  @rp:      pointer to re_compiler{}
 *  @startpp: pointer to pointer to start nfa{}
 *  @endpp:   pointer to pointer to end nfa{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static void re_comp_factor(struct re_compiler *rp,
                           struct nfa **startpp,
                           struct nfa **endpp);

void
re_compiler_init(struct re_compiler *rp, const char *re)
{
        state_stack_init(&rp->r_stk);
        rp->r_re = re;
        rp->r_p = re;
}

void
re_compiler_free(struct re_compiler *rp)
{
        state_stack_free(&rp->r_stk);
        rp->r_re = NULL;
        rp->r_p = NULL;
}

struct nfa *
re_compiler_comp(struct re_compiler *rp)
{
        struct nfa *start = NULL;
        struct nfa *end = NULL;

        re_do_comp(rp, &start, &end);

        return start;
}

static void
re_do_comp(struct re_compiler *rp, struct nfa **startpp, struct nfa **endpp)
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
        re_comp_or(rp, &start, &end);

        cur = start;
        while (*rp->r_p) {
                /**
                 *    +->EXPR-+
                 *    |       |
                 * OR-+       +->MATCH
                 *    |       |
                 *    +->EXPR-+
                 */
                re_comp_or(rp, &start2, &end2);

                /**
                 *    +->EXPR-+
                 *    |       |
                 * OR-+       +-x
                 *    |       |
                 *    +->EXPR-+
                 */
                nfa_free(&end);
                state_stack_push(&rp->r_stk);

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
re_comp_or(struct re_compiler *rp, struct nfa **startpp, struct nfa **endpp)
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
        re_comp_factor(rp, &start1, &end1);

        left = start1;
        end = end1;

        while (*rp->r_p == '|') {
                rp->r_p++;

                /**
                 * CHAR->MATCH
                 */
                re_comp_factor(rp, &start2, &end2);

                /**
                 * CHAR-x
                 * CHAR-x
                 */
                nfa_free(&end);
                nfa_free(&end2);
                state_stack_push(&rp->r_stk);
                state_stack_push(&rp->r_stk);

                /**
                 *    +->EXPR-+
                 *    |       |
                 * OR-+       +->MATCH
                 *    |       |
                 *    +->EXPR-+
                 */
                or = nfa_or_new(left, start2);
                state_stack_pop(&rp->r_stk);
                end = nfa_char_new(0);
                left->n_edges[0] = end;
                start2->n_edges[0] = end;

                left = or;
        }

        *startpp = left;
        *endpp = end;
}

static void
re_comp_factor(struct re_compiler *rp, struct nfa **startpp, struct nfa **endpp)
{
        /**
         * CHAR->MATCH
         */
        *startpp = nfa_char_new(*rp->r_p);
        *endpp = nfa_char_new(0);
        state_stack_pop(&rp->r_stk);
        state_stack_pop(&rp->r_stk);
        (*startpp)->n_edges[0] = *endpp;

        rp->r_p++;
}

int
re_compiler_nstates(const struct re_compiler *rp)
{
        return state_stack_len(&rp->r_stk);
}
