#include "machine.h"
#include "freelist.h"
#include "util.h"

/* transition */
struct trans {
        int t_from; /* source state */
        int t_to;   /* destination state */
        int t_sym;  /* symbol (-1 for epsilon) */
};

/* state machine */
struct machine {
        int          m_nstates; /* number of states */
        int          m_next;    /* next free transition */
        int          m_finish;  /* finishing state */
        struct trans m_trans[]; /* transitions */
};

/* machine{} freelist{} */
static struct freelist *g_machine_free;

struct machine *
machine_new(int nstates)
{
        static _Atomic bool init = ATOMIC_FLAG_INIT;
        struct machine *mp = NULL;
        size_t sz = 0;

        ONCE(&init, {
                g_machine_free = freelist_new("machine", 7);
        });

        sz = sizeof(*mp) + (sizeof(struct trans) * (size_t)nstates);
        mp = freelist_get(g_machine_free, sz);
        mp->m_nstates = nstates;

        return mp;
}

void
machine_free(struct machine **mpp)
{
        ASSERT(mpp != NULL);
        ASSERT(*mpp != NULL);

        freelist_put(g_machine_free, (void **)mpp);
}

void
machine_add_tran(struct machine *mp, int from, int to, int sym)
{
        struct trans *tp = NULL;

        ASSERT(mp != NULL);
        ASSERT(0 <= from && from < mp->m_nstates);
        ASSERT(0 <= to && to < mp->m_nstates);

        tp = &mp->m_trans[mp->m_next];
        tp->t_from = from;
        tp->t_to = to;
        tp->t_sym = sym;
        mp->m_next++;
}

void
machine_set_finish(struct machine *mp, int state)
{
        ASSERT(mp != NULL);

        mp->m_finish = state;
}
