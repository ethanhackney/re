#include "ptrlist.h"
#include "util.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/* ptrlist constants */
enum {
        PTRLIST_INIT_CAP = 32,
};

/**
 * key into ptrlist:
 *
 * args:
 *  @list__: pointer to ptrlist{}
 *  @hash__: hash
 */
#define ptrlist_at(list__, hash__) \
        ((list__)->pl_tab[(hash__) & ((list__)->pl_cap - 1)])

/**
 * iterate through ptr{} chain:
 *
 * args:
 *  @list__: pointer to ptrlist{}
 *  @hash__: hash
 *  @p__:    place to store struct ptr
 */
#define ptr_for_each(list__, hash__, p__) \
        for (p__ = ptrlist_at(list__, hash__); p__; p__ = p__->p_next)

/**
 * iterate through ptrlist safely:
 *
 * args:
 *  @list__:      pointer to ptrlist
 *  @p__:         place to store struct ptr 
 *  @codeblock__: code to run for each ptr
 */
#define ptrlist_for_each_safe(list__, p__, codeblock__) do {    \
        struct ptr *next__ = NULL;                              \
        struct ptr *cur__ = NULL;                               \
        size_t seen__ = 0;                                      \
        size_t bkt__ = 0;                                       \
                                                                \
        for (bkt__ = 0; seen__ < (list__)->pl_len; bkt__++) {   \
                cur__ = (list__)->pl_tab[bkt__];                \
                while (cur__) {                                 \
                        next__ = cur__->p_next;                 \
                        p__ = cur__;                            \
                        {                                       \
                                codeblock__                     \
                        }                                       \
                        cur__ = next__;                         \
                        seen__++;                               \
                }                                               \
        }                                                       \
} while (0)

/**
 * hash function:
 *
 * args:
 *  @list: pointer to ptrlist{}
 *  @ptr: pointer
 *
 * ret:
 *  hash of pointer
 */
static size_t ptrlist_hash(const struct ptrlist *list, const void *ptr);

/**
 * grow ptrlist{}:
 *
 * args:
 *  @list: pointer to ptrlist{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
static void ptrlist_grow(struct ptrlist **list);

void
ptrlist_add(struct ptrlist **list, void *ptr)
{
        struct ptrlist *ls = NULL;
        struct ptr *p = NULL;
        size_t initsize = 0;
        size_t hash = 0;
        size_t bkt = 0;

        initsize = sizeof(*list) + (sizeof(struct ptr) * PTRLIST_INIT_CAP);
        if (!*list) {
                *list = calloc(1, initsize);
                if (!*list) {
                        perror("ptrlist_add: calloc");
                        exit(1);
                }
                (*list)->pl_cap = PTRLIST_INIT_CAP;
        }
        ls = *list;

        hash = ptrlist_hash(ls, ptr);
        ptr_for_each(ls, hash, p) {
                if (p->p_ptr == ptr)
                        return;
        }

        if (ls->pl_len == ls->pl_cap) {
                ptrlist_grow(&ls);
                bkt = hash & (ls->pl_cap - 1);
        }

        p = calloc(1, sizeof(*p));
        if (!p) {
                perror("ptrlist_add: calloc");
                exit(1);
        }

        p->p_hash = hash;
        p->p_ptr = ptr;
        p->p_next = ls->pl_tab[bkt];
        ls->pl_tab[bkt] = p;
        ls->pl_len++;
        *list = ls;
}

void
ptrlist_free(struct ptrlist **list)
{
        struct ptr *p = NULL;

        ptrlist_for_each_safe(*list, p, {
                free(p);
        });

        free(*list);
        *list = NULL;
}

bool
ptrlist_has(const struct ptrlist *list, void *ptr)
{
        struct ptr *p = NULL;

        if (!list)
                return false;

        ptr_for_each(list, ptrlist_hash(list, ptr), p) {
                if (p->p_ptr == ptr)
                        return true;
        }

        return false;
}

void
ptrlist_grow(struct ptrlist **list)
{
        struct ptrlist *cur = NULL;
        struct ptrlist *new = NULL;
        struct ptr *p = NULL;
        size_t newbkt = 0;
        size_t cap = 0;

        cur = *list;
        cap = cur->pl_cap << 1;
        new = calloc(1, sizeof(*new) + (sizeof(struct ptr) * cap));
        if (!new) {
                perror("ptrlist_grow: calloc");
                exit(1);
        }
        new->pl_cap = cap;
        new->pl_len = cur->pl_len;

        ptrlist_for_each_safe(cur, p, {
                newbkt = p->p_hash & (cap - 1);
                p->p_next = new->pl_tab[newbkt];
                new->pl_tab[newbkt] = p;
        });

        free(cur);
        *list = new;
}

size_t
ptrlist_hash(const struct ptrlist *list, const void *ptr)
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

        return hash & (list->pl_cap - 1);
}
