#include "freelist.h"
#include "intset.h"
#include "util.h"
#include <stdint.h>

/* type of intset{} elements */
#if __SIZEOF_POINTER__ == 8
typedef uint64_t intset_t;
#elif __SIZEOF_POINTER__ == 4
typedef uint32_t intset_t;
#elif __SIZEOF_POINTER__ == 2
typedef uint16_t intset_t;
#else
typedef uint8_t intset_t;
#endif /* #if __SIZEOF_POINTER__ == 8) */

/* intset{} */
struct intset {
        size_t   size;  /* size of set */
        intset_t set[]; /* bitset */
};

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
}
