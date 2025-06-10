#include "freelist.h"
#include "ptrset.h"
#include "util.h"

/* ptrset{} entry */
struct ptr {
        struct ptr *p_next; /* next on hash chain */
        const void *p_ptr;  /* pointer */
};

/* ptrset{} */
struct ptrset {
        size_t      ps_size;  /* size of ps_tab */
        size_t      ps_len;   /* entry count */
        struct ptr *ps_tab[]; /* hash table */
};

/**
 * pointer hash function:
 *
 * args:
 *  @ptr: pointer
 *
 * ret:
 *  @success: hash of ptr
 *  @failure: does not
 */
static size_t ptrhash(const void *ptr);

/* ptr{} freelist{} */
static struct freelist *g_ptr_free;

/* ptrset{} freelist{} */
static struct freelist *g_ptrset_free;

struct ptrset *
ptrset_new(size_t size)
{
        static _Atomic bool init = ATOMIC_FLAG_INIT;
        struct ptrset *pp = NULL;
        size_t sz = 0;

        ASSERT(size != 0);

        ONCE(&init, {
                g_ptr_free = freelist_new("ptr", 3);
                g_ptrset_free = freelist_new("ptrset", 6);
        });

        sz = sizeof(*pp) + (sizeof(struct ptr *) * size);
        pp = freelist_get(g_ptrset_free, sz);
        pp->ps_size = size;

        return pp;
}

void
ptrset_free(struct ptrset **ppp)
{
        struct ptrset *pp = NULL;
        struct ptr *next = NULL;
        struct ptr *p = NULL;
        size_t seen = 0;
        size_t i = 0;

        ASSERT(ppp != NULL);

        pp = *ppp;
        ASSERT(pp != NULL);

        for (i = 0; seen < pp->ps_len; i++) {
                for (p = pp->ps_tab[i]; p != NULL; p = next) {
                        next = p->p_next;
                        FREE_AND_NULL(&p);
                        seen++;
                }
        }

        FREE_AND_NULL(ppp);
}

void
ptrset_add(struct ptrset *pp, const void *ptr)
{
        struct ptr *p = NULL;
        size_t hash = 0;
        size_t i = 0;

        ASSERT(pp != NULL);
        ASSERT(ptr != NULL);

        hash = ptrhash(ptr);
        i = hash % pp->ps_size;
        for (p = pp->ps_tab[i]; p != NULL; p = p->p_next) {
                if (p->p_ptr == ptr)
                        return;
        }

        p = freelist_get(g_ptr_free, sizeof(*p));
        p->p_ptr = ptr;
        p->p_next = pp->ps_tab[i];
        pp->ps_tab[i] = p;
        pp->ps_len++;
}

static size_t
ptrhash(const void *ptr)
{
        size_t *up = NULL;
        size_t uptr = (size_t)ptr;
        size_t hash = 5381;

        for (up = &uptr; up < &uptr + sizeof(size_t); up++)
                hash = ((hash << 5) + hash) + *up;

        return hash;
}

bool
ptrset_has(const struct ptrset *pp, const void *ptr)
{
        struct ptr *p = NULL;
        size_t hash = 0;
        size_t i = 0;

        ASSERT(pp != NULL);
        ASSERT(ptr != NULL);

        hash = ptrhash(ptr);
        i = hash % pp->ps_size;
        for (p = pp->ps_tab[i]; p != NULL; p = p->p_next) {
                if (p->p_ptr == ptr)
                        return true;
        }

        return false;
}
