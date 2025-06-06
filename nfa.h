#ifndef NFA_H
#define NFA_H

/* nfa{} */
struct nfa;

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
struct nfa *nfa_epsilon_new(void);

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
struct nfa *nfa_char_new(struct nfa *end, int c);

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

#endif /* #ifndef NFA_H */
