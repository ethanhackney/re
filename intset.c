#include "freelist.h"
#include "intset.h"
#include "util.h"
#include <stdint.h>

/* intset{} freelist{} */
static struct freelist *g_intset_free;

struct intset *
intset_new(size_t size)
{
        static _Atomic bool init = ATOMIC_FLAG_INIT;
        struct intset *ip = NULL;
        size_t sz = 0;

        ONCE(&init, {
                g_intset_free = freelist_new();
        });

        sz = sizeof(*ip) + (sizeof(intset_t) * size);

        return freelist_get(g_intset_free, sz);
}

void
intset_free(struct intset **ipp)
{
        ASSERT(ipp != NULL);
        ASSERT(*ipp != NULL);

        freelist_put(g_intset_free, (void **)ipp);
}
