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

        ASSERT(size != 0);

        ONCE(&init, {
                g_intset_free = freelist_new("intset", 6);
        });

        sz = sizeof(*ip) + (sizeof(intset_t) * size);
        ip = freelist_get(g_intset_free, sz);
        ip->i_size = size;

        return ip;
}

void
intset_free(struct intset **ipp)
{
        ASSERT(ipp != NULL);
        ASSERT(*ipp != NULL);

        freelist_put(g_intset_free, (void **)ipp);
}
