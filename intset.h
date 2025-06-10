#ifndef INTSET_H
#define INTSET_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "util.h"

/* type of intset{} elements */
#if __SIZEOF_POINTER__ == 8
typedef uint64_t intset_t;
#elif __SIZEOF_POINTER__ == 4
typedef uint32_t intset_t;
#elif __SIZEOF_POINTER__ == 2
typedef uint16_t intset_t;
#else
typedef uint8_t intset_t;
#endif /* #if __SIZEOF_POINTER__ == 8) */

enum {
        /* number of bits in word */
        INTSET_BITS_PER_WORD = sizeof(intset_t) << 3,
        /* log2 of number of bits per word */
        INTSET_LOG2          = __builtin_ctz(INTSET_BITS_PER_WORD),
};

/* intset{} */
struct intset {
        size_t   i_size;  /* private: size of set */
        intset_t i_set[]; /* private: bitset */
};

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

/**
 * add element to intset{}:
 *
 * args:
 *  @ip: pointer to intset{}
 *  @n:  element to add
 *
 * ret:
 *  @success: nothing
 *  @failure: does not
 */
static inline void
intset_add(struct intset *ip, int n)
{
        size_t idx = ((size_t)n) >> INTSET_LOG2;
        int bit = n & (INTSET_BITS_PER_WORD - 1);

        ASSERT(ip != NULL);

        ip->i_set[idx] |= ((intset_t)1 << bit);
}

/**
 * test if intset{} has element:
 *
 * args:
 *  @ip: pointer to intset{}
 *  @n:  element to search for
 *
 * ret:
 *  @true:  if intset{} has element
 *  @false: if not
 */
static inline bool
intset_has(const struct intset *ip, int n)
{
        size_t idx = ((size_t)n) >> INTSET_LOG2;
        int bit = n & (INTSET_BITS_PER_WORD - 1);

        ASSERT(ip != NULL);

        return ip->i_set[idx] & ((intset_t)1 << bit);
}

#endif /* #ifndef INTSET_H */
