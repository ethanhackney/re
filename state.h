#ifndef STATE_H
#define STATE_H

/* state stack */
typedef int state_stack_t;

/**
 * init state_stack_t{}:
 *
 * args:
 *  @sp: pointer to state_stack_t{}
 *
 * ret:
 *  nothing
 */
void state_stack_init(state_stack_t *sp);

/**
 * free state_stack_t{}:
 *
 * args:
 *  @sp: pointer to state_stack_t{}
 *
 * ret:
 *  nothing
 */
void state_stack_free(state_stack_t *sp);

/**
 * allocate a state{}:
 *
 * args:
 *  @sp: pointer to state_stack_t{}
 *
 * ret:
 *  nothing
 */
void state_stack_push(state_stack_t *sp);

/**
 * free a state:
 *
 * args:
 *  @sp: pointer to state_stack_t{}
 *
 * ret:
 *  nothing
 */
void state_stack_pop(state_stack_t *sp);

/**
 * get length of state_stack_t{}:
 *
 * args:
 *  @sp: pointer to state_stack_t{}
 *
 * ret:
 *  stack length
 */
int state_stack_len(const state_stack_t *sp);

#endif /* #ifndef STATE_H */
