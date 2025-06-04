#ifndef STATE_H
#define STATE_H

/**
 * allocate a state:
 *
 * args:
 *  none
 *
 * ret:
 *  nothing
 */
void state_push(void);

/**
 * free a state:
 *
 * args:
 *  none
 *
 * ret:
 *  nothing
 */
void state_pop(void);

#endif /* #ifndef STATE_H */
