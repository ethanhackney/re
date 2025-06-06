#ifndef NFA_H
#define NFA_H

#include "util.h"

/* nfa{} types */
enum {
        NFA_FIRST,   /* used for range checking */
        NFA_EPSILON, /* epsilon */
        NFA_CHAR,    /* regular character */
        NFA_COUNT,   /* type count */
};

/* nfa */
struct nfa {
        struct nfa *n_edge[2]; /* private: edges */
        int         n_type;    /* private: nfa{} type */
        int         n_c;       /* private: character */
};

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
struct nfa *nfa_new(int type);

/**
 * create an epsilon nfa{}:
 *
 * args:
 *  none
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
static inline struct nfa *
nfa_epsilon_new(void)
{
        return nfa_new(NFA_EPSILON);
}

/**
 * create a new character nfa{}:
 *
 * args:
 *  @end: end nfa{}
 *  @c:   character
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
static inline struct nfa *
nfa_char_new(struct nfa *end, int c)
{
        struct nfa *np = nfa_new(NFA_CHAR);

        np->n_c = c;
        np->n_edge[0] = end;

        return np;
}

/**
 * free an nfa{}:
 *
 * args:
 *  @npp: pointer to pointer to nfa{}
 *
 * ret:
 *  @success: *npp set to NULL
 *  @failure: does not
 */
void nfa_free(struct nfa **npp);

/**
 * get nfa{} character:
 *
 * args:
 *  @np: pointer to nfa{}
 *
 * ret:
 *  @success: nfa{} character
 *  @failure: does not
 */
static inline int
nfa_char(const struct nfa *np)
{
        ASSERT(np != NULL);

        return np->n_c;
}

/**
 * get nfa{} type:
 *
 * args:
 *  @np: pointer to nfa{}
 *
 * ret:
 *  @success: nfa{} type
 *  @failure: does not
 */
static inline int
nfa_type(const struct nfa *np)
{
        ASSERT(np != NULL);

        return np->n_type;
}

/**
 * get nfa{} edge:
 *
 * args:
 *  @np:    pointer to nfa{}
 *  @which: which edge? (0 or 1)
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: does not
 */
static inline struct nfa *
nfa_edge(const struct nfa *np, int which)
{
        ASSERT(np != NULL);
        ASSERT(which == 0 || which == 1);

        return np->n_edge[which];
}

#endif /* #ifndef NFA_H */
