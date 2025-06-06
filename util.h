#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdbool.h>

/* if debugging */
#ifdef DEBUG
#include <assert.h>
#define ASSERT(_cond) assert(_cond)
#else
#define ASSERT(_cond)
#endif /* #ifdef DEBUG */

/**
 * do something once:
 *
 * args:
 *  @_flag:  pointer to _Atomic bool
 *  @_block: code block to run
 *
 * ret:
 *  @success: nothing
 *  @failure: does not
 */
#define ONCE(_flag, _block) do {                                                \
        bool _expect = false;                                                   \
                                                                                \
        if (unlikely(atomic_compare_exchange_strong(_flag, &_expect, true)))    \
                _block                                                          \
} while (0)

/**
 * free memory and set to NULL:
 *
 * args:
 *  @_pp: pointer to pointer
 *
 * ret:
 *  @success: nothing
 *  @failure: does not
 */
#define FREE_AND_NULL(_pp) do { \
        ASSERT((_pp) != NULL);  \
        free(*(_pp));           \
        *(_pp) = NULL;          \
} while (0)

/**
 * mark condition as unlikely:
 *
 * args:
 *  @_cond: condition
 *
 * ret:
 * @true:  if condition is true
 * @false: if not
 */
#define unlikely(_cond) __builtin_expect(!!(_cond), 0)

/**
 * print message and exit:
 *
 * args:
 *  @fmt: format string
 *  @...: arguments
 *
 * ret:
 *  @success: exit process
 *  @failure: does not
 */
void die(const char *fmt, ...);

/**
 * round integer up to next power of 2:
 *
 * args:
 *  @n: integer
 *
 * ret:
 *  @success: next power of 2
 *  @failure: does not
 */
static inline size_t
next_pow2(size_t n)
{
        if (n == 0)
                return n;

        n--;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;

        switch (sizeof(n)) {
        case 2:
                n |= n >> 8;
                break;
        case 4:
                n |= n >> 8;
                n |= n >> 16;
                break;
        case 8:
                n |= n >> 8;
                n |= n >> 16;
                n |= n >> 32;
                break;
        }

        n++;
        return n;
}

/**
 * get base 2 logarithm of power of 2 integer:
 *
 * args:
 *  @n: integer
 *
 * ret:
 *  @success: base 2 logarithm
 *  @failure: does not
 */
static inline size_t
log2_pow2(size_t n)
{
        static const size_t log2_lookup[256] = {
                [ 1] = 0,
                [ 2] = 1,
                [ 4] = 2,
                [ 8] = 3,
                [16] = 4,
                [32] = 5,
                [64] = 6,
        };

        switch (sizeof(n)) {
        case 2:
                n >>= 8;
                break;
        case 4:
                n >>= 16;
                n >>= 8;
                break;
        case 8:
                n >>= 32;
                n >>= 16;
                n >>= 8;
                break;
        }

        return log2_lookup[n] + ((sizeof(n) >> 1) << 3);
}

#endif /* #ifndef UTIL_H */
