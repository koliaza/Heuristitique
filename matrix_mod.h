#ifndef MATRIX_MOD_H
#define MATRIX_MOD_H

/*** Utility functions :
     arithmetic and matrix operations modulo the Mersenne prime 2^61 - 1
     on 64-bit integers
***/

/* the suffix "p" on function names means "modulo PRIME" */

#include <stdint.h>
#include <inttypes.h>

int64_t addp(int64_t x, int64_t y);
int64_t mulp(int64_t x, int64_t y);

int64_t* matrix_addp(int n, const int64_t *a, const int64_t *b, int64_t *r);
int64_t* matrix_mulp(int n, const int64_t *a, const int64_t *b, int64_t *r);

void copy_int_array_to_int64(int n, const int *src, int64_t *dest);

#endif
