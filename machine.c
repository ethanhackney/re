#include "machine.h"

/* transition */
struct trans {
        state_t t_from; /* source state */
        state_t t_to;   /* destination state */
        int     t_sym;  /* symbol (0 for epsilon) */
};

/* state machine */
struct machine {
        state_t      m_nstates; /* number of states */
        struct trans m_trans[]; /* transitions */
};

struct machine *
machine_new(state_t nstates)
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
