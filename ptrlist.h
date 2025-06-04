#ifndef PTRLIST_H
#define PTRLIST_H

#include <stddef.h>
#include <stdbool.h>

/**
 * iterate through ptrlist:
 *
 * args:
 *  @list__:      pointer to ptrlist
 *  @p__:         place to store ptr 
 *  @codeblock__: code to run for each ptr
 */
#define ptrlist_for_each(list__, p__, codeblock__) do {         \
        struct ptr *cur__ = NULL;                               \
        size_t seen__ = 0;                                      \
        size_t bkt__ = 0;                                       \
                                                                \
        for (bkt__ = 0; seen__ < (list__)->pl_len; bkt__++) {   \
                cur__ = (list__)->pl_tab[bkt__];                \
                while (cur__) {                                 \
                        p__ = cur__->p_ptr;                     \
                        {                                       \
                                codeblock__                     \
                        }                                       \
                        cur__ = (cur__)->p_next;                \
                        seen__++;                               \
                }                                               \
        }                                                       \
} while (0)

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
 *  @list: pointer to ptrlist{}
 *  @ptr:  pointer
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
void ptrlist_add(struct ptrlist **list, void *ptr);

/**
 * free ptrlist:
 *
 * args:
 *  @list: pointer to pointer ptrlist{}
 *
 * ret:
 *  *list set to NULL
 */
void ptrlist_free(struct ptrlist **list);

/**
 * search ptrlist{}:
 *
 * args:
 *  @list: pointer ptrlist{}
 *  @ptr:  pointer to search for
 *
 * ret:
 *  @true:  if ptr in list
 *  @false: if not
 */
bool ptrlist_has(const struct ptrlist *list, void *ptr);

#endif /* #ifndef PTRLIST_H */
