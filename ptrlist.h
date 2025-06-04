#ifndef PTRLIST_H
#define PTRLIST_H

#include <stddef.h>
#include <stdbool.h>

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
