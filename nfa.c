#include "nfa.h"
#include "ptrlist.h"
#include "state.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * do nfa{} free:
 *
 * args:
 *  @np:   pointer to nfa{}
 *  @seen: seen nfa{} (prevents infinite loop)
 *
 * ret:
 *  nothing
 */
static void nfa_do_free(struct nfa *np, struct ptrlist **seen);

/**
 * do dump nfa{}:
 *
 * args:
 *  @np:    pointer to nfa{}
 *  @seen:  seen nfa{} (prevents infinite loop)
 *  @space: amount to indent
 *
 * ret:
 *  nothing
 */
static void nfa_do_dump(struct nfa *np, struct ptrlist **seen, int space);

/**
 * indent:
 *
 * args:
 *  @space: amount to indent
 *
 * ret:
 *  none
 */
static void indent(int space);

struct nfa *
nfa_new(int type)
{
        struct nfa *np = NULL;

        np = calloc(1, sizeof(*np));
        if (!np) {
                perror("nfa_new: calloc");
                exit(1);
        }

        np->n_type = type;
        state_push();
        return np;
}

void
nfa_free(struct nfa **npp)
{
        struct ptrlist *seen = NULL;

        nfa_do_free(*npp, &seen);
        *npp = NULL;

        ptrlist_free(&seen);
}

static void
nfa_do_free(struct nfa *np, struct ptrlist **seen)
{
        if (ptrlist_has(*seen, np))
                return;

        ptrlist_add(seen, np);

        switch (np->n_type) {
        case NFA_MATCH:
                break;
        case NFA_CHAR:
                nfa_do_free(np->n_edges[0], seen);
                break;
        case NFA_OR:
                nfa_do_free(np->n_edges[0], seen);
                nfa_do_free(np->n_edges[1], seen);
                break;
        default:
                fprintf(stderr, "nfa_do_free: bad nfa type: %d\n", np->n_type);
                exit(1);
        }

        free(np);
        np = NULL;
        state_pop();
}

struct nfa *
nfa_char_new(int c)
{
        struct nfa *np = NULL;

        np = nfa_new(NFA_CHAR);
        np->n_c = c;

        return np;
}

struct nfa *
nfa_match_new(void)
{
        return nfa_new(NFA_MATCH);
}

struct nfa *
nfa_or_new(struct nfa *e1, struct nfa *e2)
{
        struct nfa *np = NULL;

        np = nfa_new(NFA_OR);
        np->n_edges[0] = e1;
        np->n_edges[1] = e2;

        return np;
}

void
nfa_dump(struct nfa *np, int space)
{
        struct ptrlist *seen = NULL;

        indent(space);
        printf("{\n");

        nfa_do_dump(np, &seen, space);

        indent(space);
        printf("}\n");

        ptrlist_free(&seen);
}

void nfa_do_dump(struct nfa *np, struct ptrlist **seen, int space)
{
        if (ptrlist_has(*seen, np))
                return;

        ptrlist_add(seen, np);

        switch (np->n_type) {
        case NFA_CHAR:
                indent(space + 2);
                printf("type: NFA_CHAR,\n");

                indent(space + 2);
                printf("c: '%c',\n", np->n_c);

                indent(space + 2);
                printf("e1: {\n");

                nfa_do_dump(np->n_edges[0], seen, space + 4);

                indent(space + 2);
                printf("},\n");
                break;
        case NFA_OR:
                indent(space + 2);
                printf("type: NFA_OR,\n");

                indent(space + 2);
                printf("e1: {\n");

                nfa_do_dump(np->n_edges[0], seen, space + 4);

                indent(space + 2);
                printf("},\n");

                indent(space + 2);
                printf("e2: {\n");

                nfa_do_dump(np->n_edges[1], seen, space + 4);

                indent(space + 2);
                printf("},\n");
                break;
        case NFA_MATCH:
                indent(space + 2);
                printf("type: NFA_MATCH,\n");
                break;
        default:
                fprintf(stderr, "nfa_do_dump: bad nfa type: %d\n", np->n_type);
                exit(1);
        }
}

void
indent(int space)
{
        while (--space >= 0)
                putchar(' ');
}
