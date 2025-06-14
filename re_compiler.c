#include "re_compiler.h"
#include "freelist.h"
#include "util.h"
#include "stack.h"
#include "nfa.h"
#include <string.h>

/* regex compiler */
struct re_compiler {
        struct stack *rc_stk;   /* nfa stack */
        const char   *rc_p;     /* regex string pointer */
        int           rc_state; /* next available state */
        char          rc_re[];  /* regex string */
};

/**
 * do compile:
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
static void re_compiler_do_comp(struct re_compiler *rp,
                                struct nfa **startpp,
                                struct nfa **endpp);

/**
 * compile factor:
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
static void re_compiler_factor_comp(struct re_compiler *rp,
                                    struct nfa **startpp,
                                    struct nfa **endpp);

/* re_compiler{} freelist{} */
static struct freelist *g_re_compiler_free;

struct re_compiler *
re_compiler_new(const char *re, size_t len)
{
        static _Atomic bool init = ATOMIC_FLAG_INIT;
        struct re_compiler *rp = NULL;
        size_t sz = sizeof(*rp) + ((len + 1));

        ASSERT(re != NULL);
        ASSERT(len != 0);
        ASSERT(*re != 0);

        ONCE(&init, {
                g_re_compiler_free = freelist_new("re_compiler", 11);
        });

        rp = freelist_get(g_re_compiler_free, sz);
        rp->rc_stk = stack_new();
        strncpy(rp->rc_re, re, len);
        rp->rc_re[len] = 0;
        rp->rc_p = rp->rc_re;

        return rp;
}

void
re_compiler_free(struct re_compiler **rpp)
{
        ASSERT(rpp != NULL);
        ASSERT(*rpp != NULL);

        stack_free(&(*rpp)->rc_stk);
        freelist_put(g_re_compiler_free, (void **)rpp);
}

int
re_compiler_nstates(struct re_compiler *rp)
{
        ASSERT(rp != NULL);

        return rp->rc_state;
}

struct machine *
re_compiler_comp(struct re_compiler *rp)
{
        struct machine *mp = NULL;
        struct nfa *start = NULL;
        struct nfa *end = NULL;
        struct nfa *np = NULL;
        struct nfa *edge = NULL;
        int from = 0;
        int to = 0;
        int sym = 0;
        int nedges = 0;
        int type = 0;
        int i = 0;

        re_compiler_do_comp(rp, &start, &end);

        mp = machine_new((int)stack_len(rp->rc_stk));

        np = stack_top(rp->rc_stk);
        machine_set_finish(mp, nfa_state(np));

        while (stack_len(rp->rc_stk) > 0) {
                np = stack_pop(rp->rc_stk);

                nedges = 0;
                if (np->n_edge[0] != NULL)
                        nedges++;
                if (np->n_edge[1] != NULL)
                        nedges++;

                from = np->n_state;
                for (i = 0; i < nedges; i++) {
                        edge = nfa_edge(np, i);
                        to = nfa_state(edge);
                        type = nfa_type(edge);
                        sym = type == NFA_EPSILON ? -1 : np->n_c;

                        machine_add_tran(mp, from, to, sym);
                }
        }

        nfa_free(&start);
        return mp;
}

static void
re_compiler_do_comp(struct re_compiler *rp,
                    struct nfa **startpp,
                    struct nfa **endpp)
{
        struct nfa *start = NULL;
        struct nfa *end = NULL;
        struct nfa *start2 = NULL;
        struct nfa *end2 = NULL;
        struct nfa *cur = NULL;

        re_compiler_factor_comp(rp, &start, &end);

        cur = start;
        while (*rp->rc_p != 0) {
                re_compiler_factor_comp(rp, &start2, &end2);

                nfa_free(&end);
                (void)stack_pop(rp->rc_stk);
                rp->rc_state--;

                cur->n_edge[0] = start2;
                end = end2;
                cur = start2;
        }

        *startpp = start;
        *endpp = end;
}

static void
re_compiler_factor_comp(struct re_compiler *rp,
                        struct nfa **startpp,
                        struct nfa **endpp)
{
        struct nfa *start = NULL;
        struct nfa *end = NULL;

        end = nfa_epsilon_new(rp->rc_state);
        rp->rc_state++;
        stack_push(rp->rc_stk, end);

        start = nfa_char_new(rp->rc_state, end, *rp->rc_p);
        stack_push(rp->rc_stk, start);
        rp->rc_state++;

        rp->rc_p++;
        *startpp = start;
        *endpp = end;
}
