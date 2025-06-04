#include "nfa.h"
#include "ptrlist.h"
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
static void nfa_collect(struct nfa *np, struct ptrlist **seen);

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

/**
 * dump a NFA_CHAR:
 *
 * args:
 *  @np:   pointer to nfa{}
 *  @space: amount of indent
 *
 * ret:
 *  nothing
 */
static void nfa_char_dump(struct nfa *np, struct ptrlist **seen, int space);

/**
 * dump a NFA_OR:
 *
 * args:
 *  @np:   pointer to nfa{}
 *  @space: amount of indent
 *
 * ret:
 *  nothing
 */
static void nfa_or_dump(struct nfa *np, struct ptrlist **seen, int space);

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
        return np;
}

void
nfa_free(struct nfa **npp)
{
        struct ptrlist *seen = NULL;
        struct nfa *np = NULL;

        if (!*npp)
                return;

        nfa_collect(*npp, &seen);

        ptrlist_for_each(seen, np, {
                free(np);
        });

        ptrlist_free(&seen);
}

static void
nfa_collect(struct nfa *np, struct ptrlist **seen)
{
        if (!np)
                return;

        if (ptrlist_has(*seen, np))
                return;

        ptrlist_add(seen, np);
        nfa_collect(np->n_edges[0], seen);
        nfa_collect(np->n_edges[1], seen);
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
        static void (*dump[NFA_COUNT])(struct nfa *, struct ptrlist **, int) = {
                [NFA_CHAR] = nfa_char_dump,
                [NFA_OR]   = nfa_or_dump,
        };

        if (!np)
                return;

        if (ptrlist_has(*seen, np))
                return;

        ptrlist_add(seen, np);
        dump[np->n_type](np, seen, space);
}

void
indent(int space)
{
        while (--space >= 0)
                putchar(' ');
}

static void
nfa_char_dump(struct nfa *np, struct ptrlist **seen, int space)
{
        indent(space + 2);
        printf("type: NFA_CHAR,\n");

        indent(space + 2);
        printf("c: '%c',\n", np->n_c);

        indent(space + 2);
        printf("e1: {\n");

        nfa_do_dump(np->n_edges[0], seen, space + 4);

        indent(space + 2);
        printf("},\n");
}

static void
nfa_or_dump(struct nfa *np, struct ptrlist **seen, int space)
{
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
}
