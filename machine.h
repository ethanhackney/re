#ifndef MACHINE_H
#define MACHINE_H

#include "nfa.h"

/* misc. constants */
enum {
        /* epsilon transition */
        MACHINE_TRANS_EPSILON,
};

/* state machine */
struct machine;

/**
 * create a new machine{}:
 *
 * args:
 *  @nstates: number of states
 *
 * ret:
 *  @success: pointer to machine{}
 *  @failure: die
 */
struct machine *machine_new(state_t nstates);

/**
 * free machine{}:
 *
 * args:
 *  @mpp: pointer to pointer to machine{}
 *
 * ret:
 *  @success: *mpp set to NULL
 *  @failure: does not
 */
void machine_free(struct machine **mpp);

/**
 * add transition to machine{}
 *
 * args:
 *  @mp:   pointer to machine{}
 *  @from: source state
 *  @to:   destination state
 *  @sym:  symbol
 *
 * ret:
 *  @success: nothing
 *  @failure: does not
 */
void machine_add_tran(struct machine **mpp, int from, int to, int sym);

#endif /* #ifndef MACHINE_H */
