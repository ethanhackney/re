#include "state.h"
#include "util.h"
#include <pthread.h>
#include <errno.h>
#include <limits.h>

/* protects access to g_next_state */
static pthread_mutex_t g_next_state_mut = PTHREAD_MUTEX_INITIALIZER;

/* next available state (must be treated as a stack) */
static int g_next_state = 0;

/**
 * lock g_next_state:
 *
 * args:
 *  none
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static void state_lock(void);

/**
 * unlock g_next_state:
 *
 * args:
 *  none
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 *
 * locks needed:
 *  g_next_state_mut
 */
static void state_unlock(void);

/**
 * state overflow check:
 *
 * args:
 *  nonoe
 *
 * ret:
 *  exit process if state overflow
 *
 * locks needed:
 *  g_next_state_mut
 */
static void state_overflow_check(void);

/**
 * state underflow check:
 *
 * args:
 *  nonoe
 *
 * ret:
 *  exit process if state overflow
 *
 * locks needed:
 *  g_next_state_mut
 */
static void state_underflow_check(void);

void
state_push(void)
{
        state_lock();
        state_overflow_check();
        g_next_state++;
        state_unlock();
}

void
state_pop(void)
{
        state_lock();
        state_underflow_check();
        g_next_state--;
        state_unlock();
}

static void
state_lock(void)
{
        errno = pthread_mutex_lock(&g_next_state_mut);
        if (errno)
                die("state_lock: pthread_mutex_lock");
}

static void
state_unlock(void)
{
        errno = pthread_mutex_unlock(&g_next_state_mut);
        if (errno)
                die("state_lock: pthread_mutex_unlock");
}

static void
state_overflow_check(void)
{
        if (g_next_state < INT_MAX)
                return;

        errno = EOVERFLOW;
        die("state_push: overflow");
}

static void
state_underflow_check(void)
{
        if (g_next_state > 0)
                return;

        errno = EOVERFLOW;
        die("state_pop: underflow");
}
