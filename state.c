#include "state.h"
#include "util.h"
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>

/**
 * state overflow check:
 *
 * args:
 *  nonoe
 *
 * ret:
 *  exit process if state overflow
 */
static void state_stack_overflow_check(state_stack_t *sp);

/**
 * state underflow check:
 *
 * args:
 *  nonoe
 *
 * ret:
 *  exit process if state overflow
 */
static void state_stack_underflow_check(state_stack_t *sp);

void
state_stack_init(state_stack_t *sp)
{
        *sp = 0;
}

/**
 * free state_stack_t{}:
 *
 * args:
 *  @sp: pointer to state_stack_t{}
 *
 * ret:
 *  nothing
 */
void
state_stack_free(state_stack_t *sp)
{
        state_stack_init(sp);
}

void
state_stack_push(state_stack_t *sp)
{
        state_stack_underflow_check(sp);
        (*sp)--;
}

void
state_stack_pop(state_stack_t *sp)
{
        state_stack_overflow_check(sp);
        (*sp)++;
}

static void
state_stack_overflow_check(state_stack_t *sp)
{
        if ((size_t)(*sp + 1))
                return;

        errno = EOVERFLOW;
        die("state_push: overflow");
}

static void
state_stack_underflow_check(state_stack_t *sp)
{
        if (*sp > 0)
                return;

        errno = EOVERFLOW;
        die("state_pop: underflow");
}

int
state_stack_len(const state_stack_t *sp)
{
        return *sp;
}
