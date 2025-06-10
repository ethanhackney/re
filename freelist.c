#include "freelist.h"
#include "util.h"
#include "mutex.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/* misc. constants */
enum {
        FREELIST_BUCKET_COUNT = sizeof(size_t) << 3, /* number of buckets */
        FREELIST_THRESHOLD    = (1 << 16),           /* when to free memory */
};

/* freelist{} entry */
struct freelink {
        struct freelink *l_next; /* next on free list or alloc list */
        struct freelink *l_prev; /* previous on alloc list */
        size_t           l_size; /* bytes allocated (must be power of 2) */
};

/* freelist{} */
struct freelist {
        struct freelist *f_next;                        /* next on list */
        struct freelink *f_free[FREELIST_BUCKET_COUNT]; /* free lists */
        struct freelink  f_alloc;                       /* alloc list */
        mutex_t          f_mut;                         /* protect access */
        size_t           f_nbytes;                      /* bytes allocated */
};

static struct freelist *g_all;                  /* list of all freelist{} */
static mutex_t          g_all_mut = MUTEX_INIT; /* protext access to g_all */

/**
 * cleanup function to free g_all:
 *
 * args:
 *  none
 *
 * ret:
 *  @success: nothing
 *  @failure: does not
 */
static void freelist_cleanup(void);

/**
 * free allocated links of freelist{}:
 *
 * args:
 *  @fp: pointer to freelist{}
 *
 * ret:
 *  @success: nothing
 *  @failure: does not
 *
 * NOTE:
 *  must be called with fp->f_mut held
 */
static void freelist_free_alloc(struct freelist *fp);

/**
 * free buckets of freelist{}:
 *
 * args:
 *  @fp: pointer to freelist{}
 *
 * ret:
 *  @success: nothing
 *  @failure: does not
 *
 * NOTE:
 *  must be called with fp->f_mut held
 */
static void freelist_free_buckets(struct freelist *fp);

struct freelist *
freelist_new(void)
{
        static _Atomic bool init = ATOMIC_FLAG_INIT;
        struct freelist *fp = calloc(1, sizeof(*fp));

        ONCE(&init, {
                if (atexit(freelist_cleanup) < 0)
                        die("freelist_new: atexit");
        });

        if (fp == NULL)
                die("freelist_new: calloc");

        mutex_init(&fp->f_mut);
        fp->f_alloc.l_prev = &fp->f_alloc;
        fp->f_alloc.l_next = &fp->f_alloc;
        fp->f_alloc.l_size = 0;

        mutex_lock(&g_all_mut);
        fp->f_next = g_all;
        g_all = fp;
        mutex_unlock(&g_all_mut);

        return fp;
}

static void
freelist_cleanup(void)
{
        struct freelist *next = NULL;
        struct freelist *fp = NULL;

        for (fp = g_all; fp != NULL; fp = next) {
                next = fp->f_next;
                freelist_free_alloc(fp);
                freelist_free_buckets(fp);
                mutex_free_no_check(&fp->f_mut);
                FREE_AND_NULL(&fp);
        }
}

static void
freelist_free_alloc(struct freelist *fp)
{
        struct freelink *next = NULL;
        struct freelink *p = NULL;

        ASSERT(fp != NULL);

        for (p = fp->f_alloc.l_next; p != &fp->f_alloc; p = next) {
                next = p->l_next;
                printf("LEAK: %p\n", (void *)p);
                FREE_AND_NULL(&p);
        }
}

static void
freelist_free_buckets(struct freelist *fp)
{
        struct freelink *next = NULL;
        struct freelink *p = NULL;
        size_t i = 0;

        ASSERT(fp != NULL);

        for (i = 0; i < FREELIST_BUCKET_COUNT; i++) {
                for (p = fp->f_free[i]; p != NULL; p = next) {
                        next = p->l_next;
                        FREE_AND_NULL(&p);
                }
        }
}

void
freelist_free(struct freelist **fpp)
{
        struct freelist *fp = NULL;
        struct freelist **p = NULL;

        ASSERT(fpp != NULL);

        fp = *fpp;
        ASSERT(fp != NULL);

        freelist_free_alloc(fp);
        freelist_free_buckets(fp);
        mutex_free(&fp->f_mut);

        mutex_lock(&g_all_mut);
        for (p = &g_all; *p != NULL; p = &(*p)->f_next) {
                if (*p == fp)
                        break;
        }
        *p = fp->f_next;
        mutex_unlock(&g_all_mut);

        FREE_AND_NULL(fpp);
}

void *
freelist_get(struct freelist *fp, size_t size)
{
        size_t actual = next_pow2(size);
        size_t bucket = log2_pow2(actual);
        struct freelink *p = NULL;

        ASSERT(fp != NULL);
        ASSERT(size != 0);

        mutex_lock(&fp->f_mut);

        if (p != NULL) {
                fp->f_free[bucket] = p->l_next;
                goto done;
        }

        p = calloc(1, sizeof(*p) + actual);
        if (p == NULL)
                die("freelist_get: calloc");

        fp->f_nbytes += sizeof(*p) + actual;
        p->l_next = fp->f_alloc.l_next;
        p->l_prev = &fp->f_alloc;
        fp->f_alloc.l_next->l_prev = p;
        fp->f_alloc.l_next = p;
        p->l_size = actual;
done:
        mutex_unlock(&fp->f_mut);
        return (void *)(p + 1);
}

void
freelist_put(struct freelist *fp, void **pp)
{
        struct freelink *p = ((struct freelink *)*pp) - 1;
        size_t bucket = 0;

        ASSERT(fp != NULL);
        ASSERT(pp != NULL);
        ASSERT(*pp != NULL);

        mutex_lock(&fp->f_mut);

        if (fp->f_nbytes >= FREELIST_THRESHOLD) {
                free(*pp);
                goto done;
        }

        bucket = log2_pow2(p->l_size);
        p->l_next->l_prev = p->l_prev;
        p->l_prev->l_next = p->l_next;
        p->l_prev = NULL;
        p->l_next = fp->f_free[bucket];
        fp->f_free[bucket] = p;
done:
        mutex_unlock(&fp->f_mut);
        *pp = NULL;
}
