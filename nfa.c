#include "freelist.h"
#include "nfa.h"
#include "util.h"
#include "ptrset.h"
#include <stdatomic.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

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

/**
 * do dump:
 *
 * args:
 *  @np:    pointer to nfa{}
 *  @pp:    pointer to ptrset{}
 *  @space: amount to indent
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static void nfa_do_dump(struct nfa *np, struct ptrset *pp, int space);

/**
 * indent:
 *
 * args:
 *  @amt: amount to indent
 *
 * ret:
 *  @success: nothing
 *  @failure: does not
 */
static void indent(int amt);

/* nfa{} freelist{} */
static struct freelist *g_nfa_free;

struct nfa *
nfa_new(int state, int type)
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
        np->n_state = state;

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

void
nfa_dump(struct nfa *np)
{
        struct ptrset *pp = NULL;

        ASSERT(np != NULL);

        pp = ptrset_new(PTRSET_SIZE);

        printf("{\n");
        nfa_do_dump(np, pp, 2);
        printf("}\n");

        ptrset_free(&pp);
}

static void
nfa_do_dump(struct nfa *np, struct ptrset *pp, int space)
{
        static const char *names[NFA_COUNT] = {
                [NFA_EPSILON] = "NFA_EPSILON",
                [NFA_CHAR]    = "NFA_CHAR",
        };

        if (np == NULL)
                return;

        if (ptrset_has(pp, np))
                return;

        ptrset_add(pp, np);

        if (np->n_type < 0 || np->n_type >= NFA_COUNT) {
                errno = EINVAL;
                die("nfa_do_dump: bad nfa type: %d", np->n_type);
        }

        indent(space);
        printf("\"n_type\": \"%s\",\n", names[np->n_type]);

        indent(space);
        printf("\"n_state\": \"%lu\",\n", (uint64_t)np->n_state);

        switch (np->n_type) {
        case NFA_CHAR:
                indent(space);
                printf("\"n_c\": \"%c\",\n", np->n_c);

                indent(space);
                printf("\"n_edge[0]\": {\n");
                nfa_do_dump(np->n_edge[0], pp, space + 2);
                indent(space);
                printf("},\n");
                break;

        case NFA_EPSILON:
                indent(space);
                printf("\"n_edge[0]\": {\n");
                nfa_do_dump(np->n_edge[0], pp, space + 2);
                indent(space);
                printf("},\n");
                break;
        }
}

static void
indent(int amt)
{
        int n = 0;

        for (n = 0; n < amt; n++)
                putchar(' ');
}
