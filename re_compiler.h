#ifndef RE_COMPILER_H
#define RE_COMPILER_H

#include <stddef.h>
#include "nfa.h"
#include "state.h"

/* regex compiler */
struct re_compiler;

/**
 * create a new regex compiler:
 *
 * args:
 *  @re:  regex string
 *  @len: length of string
 *
 * ret:
 *  @success: pointer to re_compiler{}
 *  @failure: die
 */
struct re_compiler *re_compiler_new(const char *re, size_t len);

/**
 * free a regex compiler:
 *
 * args:
 *  @rpp: pointer to pointer to re_compiler{}
 *
 * ret:
 *  @success: *rpp set to NULL
 *  @failure: does not
 */
void re_compiler_free(struct re_compiler **rpp);

/**
 * get number of states:
 *
 * args:
 *  @rp: pointer to re_compiler{}
 *
 * ret:
 *  @success: *rpp set to NULL
 *  @failure: does not
 */
state_t re_compiler_nstates(struct re_compiler *rp);

/**
 * compiler regex:
 *
 * args:
 *  @rp: pointer to re_compiler{}
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *re_compiler_comp(struct re_compiler *rp);

#endif /* #ifndef RE_COMPILER_H */
