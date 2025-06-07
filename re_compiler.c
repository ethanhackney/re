#include "re_compiler.h"
#include "freelist.h"
#include "util.h"
#include <string.h>

/* regex compiler */
struct re_compiler {
        const char *rc_p;     /* regex string pointer */
        int         rc_state; /* next available state */
        char        rc_re[];  /* regex string */
};

/* re_compiler{} freelist{} */
static struct freelist *g_re_compiler_free;

struct re_compiler *
re_compiler_new(const char *re, size_t len)
{
        static _Atomic bool init = ATOMIC_FLAG_INIT;
        struct re_compiler *rp = NULL;
        size_t sz = sizeof(*rp) + ((len + 1));

        ASSERT(re != NULL);
        ASSERT(len != 0);
        ASSERT(*re != 0);

        ONCE(&init, {
                g_re_compiler_free = freelist_new();
        });

        rp = freelist_get(g_re_compiler_free, sz);
        strncpy(rp->rc_re, re, len);
        rp->rc_re[len] = 0;
        rp->rc_p = rp->rc_re;

        return rp;
}

void
re_compiler_free(struct re_compiler **rpp)
{
        ASSERT(rpp != NULL);
        ASSERT(*rpp != NULL);

        freelist_put(g_re_compiler_free, (void **)rpp);
}

int
re_compiler_nstates(struct re_compiler *rp)
{
        ASSERT(rp != NULL);

        return rp->rc_state;
}
