#ifndef NFA_H
#define NFA_H

/* nfa types */
enum {
        NFA_MATCH, /* match nfa */
        NFA_CHAR,  /* regular character */
        NFA_OR,    /* or nfa */
        NFA_COUNT, /* type count */
};

/* nfa */
struct nfa {
        struct nfa *n_edges[2]; /* nfa edges */
        int         n_type;     /* type */
        int         n_c;        /* if NFA_CHAR, character */
};

/**
 * create a new nfa{}:
 *
 * args:
 *  @type: nfa type
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *nfa_new(int type);

/**
 * allocate new nfa:
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
 * create a new char nfa{}:
 *
 * args:
 *  @c: character
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *nfa_char_new(int c);

/**
 * create a new match nfa{}:
 *
 * args:
 *  none
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *nfa_match_new(void);

/**
 * create a new or nfa{}:
 *
 * args:
 *  @e1: first edge
 *  @e2: second edge
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *nfa_or_new(struct nfa *e1, struct nfa *e2);

/**
 * dump nfa{}:
 *
 * args:
 *  @np:    pointer to nfa{}
 *  @space: amount to indent
 *
 * ret:
 *  nothing
 */
void nfa_dump(struct nfa *np, int space);

#endif /* #ifndef NFA_H */
