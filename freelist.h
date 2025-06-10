#ifndef FREELIST_H
#define FREELIST_H

#include <stddef.h>

/* freelist */
struct freelist;

/**
 * create a new freelist{}:
 *
 * args:
 *  @name: name of freelist{}
 *  @len:  length of name
 *
 * ret:
 *  @success: pointer to freelist{}
 *  @failure: die
 */
struct freelist *freelist_new(const char *name, size_t len);

/**
 * free a freelist{}:
 *
 * args:
 *  @fpp: pointer to pointer to freelist{}
 *
 * ret:
 *  @success: *fpp set to NULL
 *  @failure: die
 */
void freelist_free(struct freelist **fpp);

/**
 * allocate memory:
 *
 * args:
 *  @fp:   pointer to freelist{}
 *  @size: number of bytes to allocate
 *
 * ret:
 *  @success: pointer to allocated memory
 *  @failure: die
 */
void *freelist_get(struct freelist *fp, size_t size);

/**
 * free memory:
 *
 * args:
 *  @fp: pointer to freelist{}
 *  @pp: pointer to pointer
 *
 * ret:
 *  @success: *pp set to NULL
 *  @failure: does not
 */
void freelist_put(struct freelist *fp, void **pp);

#endif /* #ifndef FREELIST_H */
