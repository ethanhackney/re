#include "freelist.h"
#include "nfa.h"
#include "util.h"
#include <stdatomic.h>

/* nfa{} types */
enum {
        NFA_EPSILON, /* epsilon */
        NFA_CHAR,    /* regular character */
        NFA_COUNT,   /* type count */
};

/* nfa */
struct nfa {
        struct nfa *n_edge[2]; /* edges */
        int         n_type;    /* nfa{} type */
        int         n_c;       /* character if NFA_CHAR */
};

/* nfa{} freelist{} */
static struct freelist *g_nfa_free;

/**
 * create a new nfa{}:
 *
 * args:
 *  @type: nfa{} type
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
static struct nfa *nfa_new(int type);

static struct nfa *
nfa_new(int type)
{
        static atomic_flag init = ATOMIC_FLAG_INIT;
        struct nfa *np = NULL;

        if (unlikely(!atomic_flag_test_and_set(&init)))
                g_nfa_free = freelist_new();

        np = freelist_get(g_nfa_free, sizeof(*np));
        np->n_type = type;
        return np;
}

struct nfa *
nfa_epsilon_new(void)
{
        return nfa_new(NFA_EPSILON);
}

struct nfa *
nfa_char_new(struct nfa *end, int c)
{
        struct nfa *np = nfa_new(NFA_CHAR);

        np->n_c = c;
        np->n_edge[0] = end;

        return np;
}

void
nfa_free(struct nfa **npp)
{
        freelist_put(g_nfa_free, (void **)npp);
}
