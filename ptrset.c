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
        size_t     ps_size;  /* size of ps_tab */
        struct ptr ps_tab[]; /* hash table */
};

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

        return freelist_get(g_ptrset_free, sz);
}

void
ptrset_free(struct ptrset **ppp)
{
}

void
ptrset_add(struct ptrset *pp, const void *ptr)
{
}

bool
ptrset_has(const struct ptrset *pp, const void *ptr)
{
        return true;
}
