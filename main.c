#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

/* protects access to g_next_state */
static pthread_mutex_t g_next_state_mut = PTHREAD_MUTEX_INITIALIZER;

/* next available state (must be treated as a stack) */
static int g_next_state = 0;

/**
 * allocate a state:
 *
 * args:
 *  none
 *
 * ret:
 *  nothing
 */
void state_pop(void);

/**
 * free a state:
 *
 * args:
 *  none
 *
 * ret:
 *  nothing
 */
void state_push(void);

/**
 * lock state allocator:
 *
 * args:
 *  none
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 *
 * locks needed:
 *  g_next_state_mut
 */
void state_lock(void);

/**
 * unlock state allocator:
 *
 * args:
 *  none
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 *
 * locks needed:
 *  g_next_state_mut
 */
void state_unlock(void);

/**
 * state overflow check:
 *
 * args:
 *  nonoe
 *
 * ret:
 *  exit process if state overflow
 *
 * locks needed:
 *  g_next_state_mut
 */
void state_overflow_check(void);

/**
 * state underflow check:
 *
 * args:
 *  nonoe
 *
 * ret:
 *  exit process if state overflow
 *
 * locks needed:
 *  g_next_state_mut
 */
void state_underflow_check(void);

/**
 * die:
 *
 * args:
 *  @fmt: format string
 *  @...: args
 *
 * ret:
 *  exit process
 */
void die(const char *fmt, ...);

/* ptrlist constants */
enum {
        PTRLIST_INIT_CAP = 32,
};

/* nfa types */
enum {
        NFA_MATCH, /* match nfa */
        NFA_CHAR,  /* regular character */
        NFA_OR,    /* or nfa */
        NFA_COUNT, /* type count */
};

/* ptrlist entry */
struct ptr {
        struct ptr *p_next; /* next on hash chain */
        size_t      p_hash; /* saved hash */
        void       *p_ptr;  /* pointer */
};

/* list of pointers */
struct ptrlist {
        size_t      pl_cap;   /* capacity */
        size_t      pl_len;   /* length */
        struct ptr *pl_tab[]; /* hash table */
};

/**
 * add pointer to start of list:
 *
 * args:
 *  @head: head of list
 *  @ptr:  pointer
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
void ptrlist_add(struct ptrlist **head, void *ptr);

/**
 * hash function:
 *
 * args:
 *  @ptr: pointer
 *
 * ret:
 *  hash of pointer
 */
size_t ptrlist_hash(const void *ptr);

/**
 * grow ptrlist{}:
 *
 * args:
 *  @head: head of list
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
void ptrlist_grow(struct ptrlist **head);

/**
 * free ptrlist:
 *
 * args:
 *  @head: head of list
 *
 * ret:
 *  nothing
 */
void ptrlist_free(struct ptrlist **head);

/**
 * search ptrlist{}:
 *
 * args:
 *  @head: head of list
 *  @ptr:  pointer to search for
 *
 * ret:
 *  @true:  if ptr in list
 *  @false: if not
 */
bool ptrlist_has(const struct ptrlist *head, void *ptr);

/* nfa */
struct nfa {
        struct nfa *n_edges[2]; /* nfa edges */
        int         n_type;     /* type */
        int         n_c;        /* if NFA_CHAR, character */
};

/**
 * create a new nfa{}:
 *
 * args:
 *  @type: nfa type
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *nfa_new(int type);

/**
 * allocate new nfa:
 *
 * args:
 *  @npp: pointer to pointer to nfa{}
 *
 * ret:
 *  @success: *npp set to NULL
 *  @failure: does not
 */
void nfa_free(struct nfa **npp);

/**
 * do nfa{} free:
 *
 * args:
 *  @np:   pointer to nfa{}
 *  @seen: seen nfa{} (prevents infinite loop)
 *
 * ret:
 *  nothing
 */
void nfa_do_free(struct nfa *np, struct ptrlist **seen);

/**
 * create a new char nfa{}:
 *
 * args:
 *  @c: character
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *nfa_char_new(int c);

/**
 * create a new match nfa{}:
 *
 * args:
 *  none
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *nfa_match_new(void);

/**
 * create a new or nfa{}:
 *
 * args:
 *  @e1: first edge
 *  @e2: second edge
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *nfa_or_new(struct nfa *e1, struct nfa *e2);

/**
 * compile regex:
 *
 * args:
 *  @re: regex
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *re_comp(const char *re);

/**
 * do compile regex:
 *
 * args:
 *  @spp: pointer to pointer to string
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *re_do_comp(const char **spp);

/**
 * compile or expression:
 *
 * args:
 *  @spp:     pointer to pointer to string
 *  @startpp: pointer to pointer to start nfa{}
 *  @endpp:   pointer to pointer to end nfa{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
void re_comp_or(const char **spp, struct nfa **startpp, struct nfa **endpp);

/**
 * compile regex factor:
 *
 * args:
 *  @spp:     pointer to pointer to string
 *  @startpp: pointer to pointer to start nfa{}
 *  @endpp:   pointer to pointer to end nfa{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
void re_comp_factor(const char **spp, struct nfa **startpp, struct nfa **endpp);

/**
 * dump nfa{}:
 *
 * args:
 *  @np:    pointer to nfa{}
 *  @space: amount to indent
 *
 * ret:
 *  nothing
 */
void nfa_dump(struct nfa *np, int space);

/**
 * do dump nfa{}:
 *
 * args:
 *  @np:    pointer to nfa{}
 *  @seen:  seen nfa{} (prevents infinite loop)
 *  @space: amount to indent
 *
 * ret:
 *  nothing
 */
void nfa_do_dump(struct nfa *np, struct ptrlist **seen, int space);

/**
 * indent:
 *
 * args:
 *  @space: amount to indent
 *
 * ret:
 *  none
 */
void indent(int space);

int
main(void)
{
        struct nfa *np = NULL;

        /**
         *          +->CHAR-+
         *          |       |
         * CHAR->OR-+       +->MATCH
         *          |       |
         *          +->CHAR-+
         */
        np = re_comp("ab|c|d");
        nfa_dump(np, 0);
        nfa_free(&np);
}

struct nfa *
nfa_new(int type)
{
        struct nfa *np = NULL;

        np = calloc(1, sizeof(*np));
        if (!np) {
                perror("nfa_new: calloc");
                exit(1);
        }

        np->n_type = type;
        state_push();
        return np;
}

void
nfa_free(struct nfa **npp)
{
        struct ptrlist *seen = NULL;

        nfa_do_free(*npp, &seen);
        *npp = NULL;

        ptrlist_free(&seen);
}

void
nfa_do_free(struct nfa *np, struct ptrlist **seen)
{
        if (ptrlist_has(*seen, np))
                return;

        ptrlist_add(seen, np);

        switch (np->n_type) {
        case NFA_MATCH:
                break;
        case NFA_CHAR:
                nfa_do_free(np->n_edges[0], seen);
                break;
        case NFA_OR:
                nfa_do_free(np->n_edges[0], seen);
                nfa_do_free(np->n_edges[1], seen);
                break;
        default:
                fprintf(stderr, "nfa_do_free: bad nfa type: %d\n", np->n_type);
                exit(1);
        }

        free(np);
        np = NULL;
        state_pop();
}

struct nfa *
nfa_char_new(int c)
{
        struct nfa *np = NULL;

        np = nfa_new(NFA_CHAR);
        np->n_c = c;

        return np;
}

struct nfa *
nfa_match_new(void)
{
        return nfa_new(NFA_MATCH);
}

struct nfa *
nfa_or_new(struct nfa *e1, struct nfa *e2)
{
        struct nfa *np = NULL;

        np = nfa_new(NFA_OR);
        np->n_edges[0] = e1;
        np->n_edges[1] = e2;

        return np;
}

struct nfa *
re_comp(const char *re)
{
        return re_do_comp(&re);
}

struct nfa *
re_do_comp(const char **spp)
{
        struct nfa *start2 = NULL;
        struct nfa *end2 = NULL;
        struct nfa *start = NULL;
        struct nfa *end = NULL;
        struct nfa *cur = NULL;

        /**
         *    +->CHAR-+
         *    |       |
         * OR-+       +->MATCH
         *    |       |
         *    +->CHAR-+
         */
        re_comp_or(spp, &start, &end);

        cur = start;
        while (**spp) {
                /**
                 *    +->CHAR-+
                 *    |       |
                 * OR-+       +->MATCH
                 *    |       |
                 *    +->CHAR-+
                 */
                re_comp_or(spp, &start2, &end2);

                /**
                 *    +->CHAR-+
                 *    |       |
                 * OR-+       +-x
                 *    |       |
                 *    +->CHAR-+
                 */
                nfa_free(&end);

                /**
                 *    +->CHAR-+     +->CHAR-+
                 *    |       |     |       |
                 * OR-+       +->OR-+       +->MATCH
                 *    |       |     |       |
                 *    +->CHAR-+     +->CHAR-+
                 */
                cur->n_edges[0] = start2;
                cur = start2;
                end = end2;
        }

        return start;
}

void
re_comp_or(const char **spp, struct nfa **startpp, struct nfa **endpp)
{
        struct nfa *start2 = NULL;
        struct nfa *end2 = NULL;
        struct nfa *start1 = NULL;
        struct nfa *end1 = NULL;
        struct nfa *end = NULL;
        struct nfa *or = NULL;
        struct nfa *left = NULL;

        /**
         * CHAR->MATCH
         */
        re_comp_factor(spp, &start1, &end1);

        left = start1;
        end = end1;

        while (**spp == '|') {
                (*spp)++;

                /**
                 * CHAR->MATCH
                 */
                re_comp_factor(spp, &start2, &end2);

                /**
                 * CHAR-x
                 * CHAR-x
                 */
                nfa_free(&end);
                nfa_free(&end2);

                /**
                 *    +->CHAR-+
                 *    |       |
                 * OR-+       +->MATCH
                 *    |       |
                 *    +->CHAR-+
                 */
                or = nfa_or_new(left, start2);
                end = nfa_match_new();
                start1->n_edges[0] = end;
                start2->n_edges[0] = end;

                left = or;
        }

        *startpp = left;
        *endpp = end;
}

void
re_comp_factor(const char **spp, struct nfa **startpp, struct nfa **endpp)
{
        struct nfa *start = NULL;
        struct nfa *end = NULL;

        /**
         * CHAR->MATCH
         */
        start = nfa_char_new(**spp);
        end = nfa_match_new();
        start->n_edges[0] = end;

        *startpp = start;
        *endpp = end;
        (*spp)++;
}

void
nfa_dump(struct nfa *np, int space)
{
        struct ptrlist *seen = NULL;

        indent(space);
        printf("{\n");

        nfa_do_dump(np, &seen, space);

        indent(space);
        printf("}\n");

        ptrlist_free(&seen);
}

void nfa_do_dump(struct nfa *np, struct ptrlist **seen, int space)
{
        if (ptrlist_has(*seen, np))
                return;

        ptrlist_add(seen, np);

        switch (np->n_type) {
        case NFA_CHAR:
                indent(space + 2);
                printf("type: NFA_CHAR,\n");

                indent(space + 2);
                printf("c: '%c',\n", np->n_c);

                indent(space + 2);
                printf("e1: {\n");

                nfa_do_dump(np->n_edges[0], seen, space + 4);

                indent(space + 2);
                printf("},\n");
                break;
        case NFA_OR:
                indent(space + 2);
                printf("type: NFA_OR,\n");

                indent(space + 2);
                printf("e1: {\n");

                nfa_do_dump(np->n_edges[0], seen, space + 4);

                indent(space + 2);
                printf("},\n");

                indent(space + 2);
                printf("e2: {\n");

                nfa_do_dump(np->n_edges[1], seen, space + 4);

                indent(space + 2);
                printf("},\n");
                break;
        case NFA_MATCH:
                indent(space + 2);
                printf("type: NFA_MATCH,\n");
                break;
        default:
                fprintf(stderr, "nfa_do_dump: bad nfa type: %d\n", np->n_type);
                exit(1);
        }
}

void
indent(int space)
{
        while (--space >= 0)
                putchar(' ');
}

void
ptrlist_add(struct ptrlist **head, void *ptr)
{
        struct ptrlist *list = NULL;
        struct ptr *p = NULL;
        size_t initsize = 0;
        size_t hash = 0;
        size_t bkt = 0;

        initsize = sizeof(*list) + (sizeof(struct ptr) * PTRLIST_INIT_CAP);
        list = *head;
        if (!list) {
                list = calloc(1, initsize);
                if (!list) {
                        perror("ptrlist_add: calloc");
                        exit(1);
                }
                list->pl_cap = PTRLIST_INIT_CAP;
        }

        hash = ptrlist_hash(ptr);
        bkt = hash & (list->pl_cap - 1);
        for (p = list->pl_tab[bkt]; p; p = p->p_next) {
                if (p->p_ptr == ptr)
                        return;
        }

        if (list->pl_len == list->pl_cap) {
                ptrlist_grow(&list);
                bkt = hash & (list->pl_cap - 1);
        }

        p = calloc(1, sizeof(*p));
        if (!p) {
                perror("ptrlist_add: calloc");
                exit(1);
        }

        p->p_hash = hash;
        p->p_ptr = ptr;
        p->p_next = list->pl_tab[bkt];
        list->pl_tab[bkt] = p;
        list->pl_len++;
        *head = list;
}

void
ptrlist_free(struct ptrlist **head)
{
        struct ptrlist *list = NULL;
        struct ptr *next = NULL;
        struct ptr *p = NULL;
        size_t seen = 0;
        size_t bkt = 0;

        list = *head;
        for (bkt = 0; seen < list->pl_len; bkt++) {
                for (p = list->pl_tab[bkt]; p; p = next) {
                        next = p->p_next;
                        free(p);
                        p = NULL;
                        seen++;
                }
        }

        free(list);
        *head = NULL;
}

bool
ptrlist_has(const struct ptrlist *head, void *ptr)
{
        struct ptr *p = NULL;
        size_t hash = 0;
        size_t bkt = 0;

        if (!head)
                return false;

        hash = ptrlist_hash(ptr);
        bkt = hash & (head->pl_cap - 1);
        for (p = head->pl_tab[bkt]; p; p = p->p_next) {
                if (p->p_ptr == ptr)
                        return true;
        }

        return false;
}

void
ptrlist_grow(struct ptrlist **head)
{
        struct ptrlist *cur = NULL;
        struct ptrlist *new = NULL;
        struct ptr *next = NULL;
        struct ptr *p = NULL;
        size_t newbkt = 0;
        size_t seen = 0;
        size_t cap = 0;
        size_t bkt = 0;

        cur = *head;
        cap = cur->pl_cap << 1;
        new = calloc(1, sizeof(*new) + (sizeof(struct ptr) * cap));
        if (!new) {
                perror("ptrlist_grow: calloc");
                exit(1);
        }
        new->pl_cap = cap;
        new->pl_len = cur->pl_len;

        cur = *head;
        seen = 0;
        for (bkt = 0; seen < cur->pl_len; bkt++) {
                for (p = cur->pl_tab[bkt]; p; p = next) {
                        next = p->p_next;
                        newbkt = p->p_hash & (cap - 1);
                        p->p_next = new->pl_tab[newbkt];
                        new->pl_tab[newbkt] = p;
                        seen++;
                }
        }

        free(cur);
        *head = new;
}

size_t
ptrlist_hash(const void *ptr)
{
        const uint8_t *p = NULL;
        size_t nptr = 0;
        size_t hash = 0;
        size_t n = 0;

        nptr = (size_t)(ptr);
        p = (const uint8_t *)&nptr;
        n = sizeof(ptr);
        hash = 0;

        while (n-- > 0)
                hash = ((hash << 5) - hash) * *p++;

        return hash;
}

void
state_pop(void)
{
        state_lock();
        state_overflow_check();
        g_next_state++;
        state_unlock();
}

void
state_push(void)
{
        state_lock();
        state_underflow_check();
        g_next_state--;
        state_unlock();
}

void
state_lock(void)
{
        errno = pthread_mutex_lock(&g_next_state_mut);
        if (errno)
                die("state_lock: pthread_mutex_lock");
}

void
state_unlock(void)
{
        errno = pthread_mutex_unlock(&g_next_state_mut);
        if (errno)
                die("state_lock: pthread_mutex_unlock");
}

void
die(const char *fmt, ...)
{
        va_list args;
        int error = 0;

        error = errno;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
        fprintf(stderr, ": %s\n", strerror(error));
        exit(1);
}

void
state_overflow_check(void)
{
        if (g_next_state < INT_MAX)
                return;

        errno = EOVERFLOW;
        die("state_push: overflow");
}

void
state_underflow_check(void)
{
        if (g_next_state > 0)
                return;

        errno = EOVERFLOW;
        die("state_pop: underflow");
}
