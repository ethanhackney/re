#ifndef INTSET_H
#define INTSET_H

#include <stddef.h>

/* intset{} */
struct intset;

/**
 * create a new intset{}:
 *
 * args:
 *  @size: size of intset{}
 *
 * ret:
 *  @success: pointer to intset{}
 *  @failure: die
 */
struct intset *intset_new(size_t size);

/**
 * free an intset{}:
 *
 * args:
 *  @ipp: pointer to pointer to intset{}
 *
 * ret:
 *  @success: *ipp set to NULL
 *  @failure: does not
 */
void intset_free(struct intset **ipp);

#endif /* #ifndef INTSET_H */
