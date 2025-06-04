#ifndef COMP_H
#define COMP_H

/**
 * compile regex:
 *
 * args:
 *  @re: regex
 *
 * ret:
 *  @success: pointer to nf{}
 *  @failure: die
 */
struct nfa *re_comp(const char *re);

#endif /* #ifndef COMP_H */
