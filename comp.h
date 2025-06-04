#ifndef COMP_H
#define COMP_H

#include "state.h"
#include "nfa.h"

/* regex compiler */
struct re_compiler {
        state_stack_t  r_stk; /* state stack */
        const char    *r_re;  /* regex */
        const char    *r_p;   /* next char */
};

/**
 * init re_compiler{}:
 *
 * args:
 *  @rp: pointer to re_compiler{}
 *  @re: regex
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
void re_compiler_init(struct re_compiler *rp, const char *re);

/**
 * free re_compiler{}:
 *
 * args:
 *  @rp: pointer to re_compiler{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
void re_compiler_free(struct re_compiler *rp);

/**
 * get number of states:
 *
 * args:
 *  @rp: pointer to re_compiler{}
 *
 * ret:
 *  @success: number of states
 *  @failure: die
 */
int re_compiler_nstates(const struct re_compiler *rp);

/**
 * compile regex:
 *
 * args:
 *  @rp: pointer to re_compiler{}
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *re_compiler_comp(struct re_compiler *rp);

#endif /* #ifndef COMP_H */
