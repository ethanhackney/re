#include "freelist.h"
#include "util.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* misc. constants */
enum {
        FREELIST_BUCKET_COUNT = sizeof(size_t) << 3, /* number of buckets */
        FREELIST_THRESHOLD    = (1 << 16),           /* when to free memory */
};

/* freelist{} entry */
struct freelink {
        struct freelink *l_next; /* next on free list */
        size_t           l_size; /* bytes allocated (must be power of 2) */
};

/* freelist{} */
struct freelist {
        struct freelink *f_free[FREELIST_BUCKET_COUNT]; /* free lists */
        pthread_mutex_t  f_mut;                         /* protect access */
        size_t           f_nbytes;                      /* bytes allocated */
};

/**
 * lock freelist:
 *
 * args:
 *  @fp: pointer to freelist{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static void freelist_lock(struct freelist *fp);

/**
 * unlock freelist:
 *
 * args:
 *  @fp: pointer to freelist{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static void freelist_unlock(struct freelist *fp);

struct freelist *
freelist_new(void)
{
        struct freelist *fp = calloc(1, sizeof(*fp));

        if (fp == NULL)
                die("freelist_new: calloc");

        errno = pthread_mutex_init(&fp->f_mut, NULL);
        if (errno != 0)
                die("freelist_new: pthread_mutex_init");

        return fp;
}

void
freelist_free(struct freelist **fpp)
{
        struct freelist *fp = *fpp;
        struct freelink *next = NULL;
        struct freelink *p = NULL;
        size_t i = 0;

        errno = pthread_mutex_destroy(&fp->f_mut);
        if (errno != 0)
                die("freelist_free: pthread_mutex_destroy");

        for (i = 0; i < FREELIST_BUCKET_COUNT; i++) {
                for (p = fp->f_free[i]; p != NULL; p = next) {
                        next = p->l_next;
                        free(p);
                }
        }

        free(fp);
        *fpp = NULL;
}

void *
freelist_get(struct freelist *fp, size_t size)
{
        size_t actual = next_pow2(size);
        size_t bucket = log2_pow2(actual);
        struct freelink *p = NULL;

        freelist_lock(fp);

        if (p != NULL) {
                fp->f_free[bucket] = p->l_next;
                goto done;
        }

        p = calloc(1, sizeof(*p) + actual);
        if (p == NULL)
                die("freelist_get: calloc");

        fp->f_nbytes += sizeof(*p) + actual;
        p->l_size = actual;
done:
        freelist_unlock(fp);
        return (void *)(p + 1);
}

static void
freelist_lock(struct freelist *fp)
{
        errno = pthread_mutex_lock(&fp->f_mut);
        if (errno != 0)
                die("freelist_lock: pthread_mutex_lock");
}

static void
freelist_unlock(struct freelist *fp)
{
        errno = pthread_mutex_unlock(&fp->f_mut);
        if (errno != 0)
                die("freelist_unlock: pthread_mutex_unlock");
}

void
freelist_put(struct freelist *fp, void **pp)
{
        struct freelink *p = ((struct freelink *)*pp) - 1;
        size_t bucket = 0;

        freelist_lock(fp);

        if (fp->f_nbytes >= FREELIST_THRESHOLD) {
                free(*pp);
                goto done;
        }

        bucket = log2_pow2(p->l_size);
        p->l_next = fp->f_free[bucket];
        fp->f_free[bucket] = p;
done:
        freelist_unlock(fp);
        *pp = NULL;
}
