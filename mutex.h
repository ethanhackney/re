#ifndef MUTEX_H
#define MUTEX_H

#include "util.h"
#include <errno.h>
#include <pthread.h>

/* mutex type */
typedef pthread_mutex_t mutex_t;

/* statically initialize mutex_t{} */
#define MUTEX_INIT PTHREAD_MUTEX_INITIALIZER

/**
 * dynamcally initialize mutex_t{}:
 *
 * args:
 *  @mp: pointer to mutex_t{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static inline void
mutex_init(mutex_t *mp)
{
        errno = pthread_mutex_init(mp, NULL);
        if (errno != 0)
                die("mutex_init: pthread_mutex_init");
}

/**
 * free mutex_t{}, but do not check if it worked:
 *
 * args:
 *  @mp: pointer to mutex_t{}
 *
 * ret:
 *  @success: nothing
 *  @failure: does not
 */
static inline void
mutex_free_no_check(mutex_t *mp)
{
        errno = pthread_mutex_destroy(mp);
}

/**
 * free mutex_t{}:
 *
 * args:
 *  @mp: pointer to mutex_t{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static inline void
mutex_free(mutex_t *mp)
{
        mutex_free_no_check(mp);
        if (errno != 0)
                die("mutex_destroy: pthread_mutex_destroy");
}

/**
 * lock mutex_t{}:
 *
 * args:
 *  @mp: pointer to mutex_t{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static inline void
mutex_lock(mutex_t *mp)
{
        errno = pthread_mutex_lock(mp);
        if (errno != 0)
                die("mutex_lock: pthread_mutex_lock");
}

/**
 * unlock mutex_t{}:
 *
 * args:
 *  @mp: pointer to mutex_t{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static inline void
mutex_unlock(mutex_t *mp)
{
        errno = pthread_mutex_unlock(mp);
        if (errno != 0)
                die("mutex_unlock: pthread_mutex_unlock");
}

#endif /* #ifndef MUTEX_H */
