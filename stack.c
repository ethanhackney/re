#include "stack.h"
#include "freelist.h"
#include "util.h"

/* stack element */
struct stack_elem {
        struct stack_elem *e_next;
        const void        *e_elem;
};

/* stack */
struct stack {
        struct stack_elem *s_top;
};

/* stack_elem{} freelist{} */
static struct freelist *g_stack_elem_free;

/* stack{} freelist{} */
static struct freelist *g_stack_free;

struct stack *
stack_new(void)
{
        static _Atomic bool init = ATOMIC_FLAG_INIT;
        struct stack *sp = NULL;

        ONCE(&init, {
                g_stack_free = freelist_new("stack", 5);
                g_stack_elem_free = freelist_new("stack_elem", 10);
        });

        return freelist_get(g_stack_free, sizeof(*sp));
}

void
stack_free(struct stack **spp)
{
        struct stack_elem *next = NULL;
        struct stack_elem *p = NULL;

        ASSERT(spp != NULL);
        ASSERT(*spp != NULL);

        for (p = (*spp)->s_top; p != NULL; p = next) {
                next = p->e_next;
                freelist_put(g_stack_elem_free, (void **)&p);
        }

        freelist_put(g_stack_free, (void **)spp);
}

void
stack_push(struct stack *sp, const void *elem)
{
        struct stack_elem *ep = NULL;

        ASSERT(sp != NULL);
        ASSERT(elem != NULL);

        ep = freelist_get(g_stack_elem_free, sizeof(*ep));
        ep->e_elem = elem;
        ep->e_next = sp->s_top;
        sp->s_top = ep;
}

void *
stack_top(struct stack *sp)
{
        ASSERT(sp != NULL);
        ASSERT(sp->s_top != NULL);

        return (void *)sp->s_top->e_elem;
}

void *
stack_pop(struct stack *sp)
{
        struct stack_elem *top = NULL;
        const void *elem = NULL;

        ASSERT(sp != NULL);
        ASSERT(sp->s_top != NULL);

        top = sp->s_top;
        elem = top->e_elem;
        sp->s_top = top->e_next;

        freelist_put(g_stack_elem_free, (void **)&top);

        return (void *)elem;
}
