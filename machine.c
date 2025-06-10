#include "machine.h"

/* transition */
struct trans {
        int t_from; /* source state */
        int t_to;   /* destination state */
        int t_sym;  /* symbol (-1 for epsilon) */
};

/* state machine */
struct machine {
        int          m_nstates; /* number of states */
        struct trans m_trans[]; /* transitions */
};

struct machine *
machine_new(int nstates)
{
        return NULL;
}

void
machine_free(struct machine **mpp)
{
}

void
machine_add_tran(struct machine **mpp, int from, int to, int sym)
{
}
