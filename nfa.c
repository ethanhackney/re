#include "freelist.h"
#include "nfa.h"
#include "util.h"
#include "ptrset.h"
#include <stdatomic.h>

/* misc. constants */
enum {
        /* size of ptrset{} */
        PTRSET_SIZE = 117,
};

/**
 * do free:
 *
 * args:
 *  @np: pointer to nfa{}
 *  @pp: pointer to ptrset{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static void nfa_do_free(struct nfa *np, struct ptrset *pp);

/* nfa{} freelist{} */
static struct freelist *g_nfa_free;

struct nfa *
nfa_new(int type)
{
        static _Atomic bool init = ATOMIC_FLAG_INIT;
        struct nfa *np = NULL;

        ASSERT(NFA_FIRST < type);
        ASSERT(type < NFA_COUNT);

        ONCE(&init, {
                g_nfa_free = freelist_new("nfa", 3);
        });

        np = freelist_get(g_nfa_free, sizeof(*np));
        np->n_type = type;

        return np;
}

void
nfa_free(struct nfa **npp)
{
        struct ptrset *pp = NULL;

        ASSERT(npp != NULL);
        ASSERT(*npp != NULL);

        pp = ptrset_new(PTRSET_SIZE);
        nfa_do_free(*npp, pp);

        ptrset_free(&pp);
        *npp = NULL;
}

static void
nfa_do_free(struct nfa *np, struct ptrset *pp)
{
        if (np == NULL)
                return;

        if (ptrset_has(pp, np))
                return;

        ptrset_add(pp, np);
        nfa_do_free(np->n_edge[0], pp);
        nfa_do_free(np->n_edge[1], pp);
        freelist_put(g_nfa_free, (void **)&np);
}
