#ifndef STACK_H
#define STACK_H

#include <stddef.h>

/* stack */
struct stack;

/**
 * create a new stack{}:
 *
 * args:
 *  none
 *
 * ret:
 *  @success: pointer to stack{}
 *  @failure: die
 */
struct stack *stack_new(void);

/**
 * create a stack{}:
 *
 * args:
 *  @spp: pointer to pointer to stack{}
 *
 * ret:
 *  @success: *spp set to NULL
 *  @failure: does not
 */
void stack_free(struct stack **spp);

/**
 * push onto stack{}:
 *
 * args:
 *  @sp:   pointer to stack{}
 *  @elem: element to push
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
void stack_push(struct stack *sp, const void *elem);

/**
 * get top of stack{}:
 *
 * args:
 *  @sp:   pointer to stack{}
 *
 * ret:
 *  @success: element
 *  @failure: does not
 */
void *stack_top(struct stack *sp);

/**
 * get length of stack{}:
 *
 * args:
 *  @sp:   pointer to stack{}
 *
 * ret:
 *  @success: length of stack
 *  @failure: does not
 */
size_t stack_len(struct stack *sp);

/**
 * pop from stack{}:
 *
 * args:
 *  @sp:   pointer to stack{}
 *
 * ret:
 *  @success: element
 *  @failure: die
 */
void *stack_pop(struct stack *sp);

#endif /* #ifndef STACK_H */
