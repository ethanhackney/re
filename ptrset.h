#ifndef PTRSET_H
#define PTRSET_H

#include <stddef.h>
#include <stdbool.h>

/* ptrset{} */
struct ptrset;

/**
 * create a new ptrset{}:
 *
 * args:
 *  @size: size of ptrset{}
 *
 * ret:
 *  @success: pointer to ptrset{}
 *  @failure: die
 */
struct ptrset *ptrset_new(size_t size);

/**
 * free a ptrset{}:
 *
 * args:
 *  @ppp: pointer to pointer to ptrset{}
 *
 * ret:
 *  @success: *ppp set to NULL
 *  @failure: does not
 */
void ptrset_free(struct ptrset **ppp);

/**
 * add element to ptrset{}:
 *
 * args:
 *  @pp:  pointer to ptrset{}
 *  @ptr: pointer to add
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
void ptrset_add(struct ptrset *pp, const void *ptr);

/**
 * test if element is in ptrset{}:
 *
 * args:
 *  @pp:  pointer to ptrset{}
 *  @ptr: pointer to find
 *
 * ret:
 *  @true:  if ptr is in set
 *  @false: if not
 */
bool ptrset_has(const struct ptrset *pp, const void *ptr);

#endif /* #ifndef PTRSET_H */
