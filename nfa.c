#include "freelist.h"
#include "nfa.h"
#include "util.h"
#include <stdatomic.h>

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
        ASSERT(npp != NULL);
        ASSERT(*npp != NULL);

        freelist_put(g_nfa_free, (void **)npp);
}
