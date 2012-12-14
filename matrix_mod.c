#include "matrix_mod.h"

const int64_t PRIME = (((int64_t)1) << 61) - 1;
const int64_t PRIMESMALL = (((int64_t)1) << 31) - 1;
// to find the primes : http://primes.utm.edu/lists/2small/0bit.html

/* pray and hope it's inlined */
int64_t addp(int64_t x, int64_t y) {
    return ((x+y) % PRIME);
}


int64_t mulp(int64_t x, int64_t y) {
    int i;

    /* Note : if x and y are already < PRIME,
       they take at most 61 bits which we separate into
       31 low bits and 30 high bits */
    int64_t xl = x & 0x7FFFFFFF; /* 2^31-1 */
    int64_t xh = x >> 31;
    int64_t yl = y & 0x7FFFFFFF;
    int64_t yh = y >> 31;

    int64_t mid = xh * yl + xl * yh;
    int64_t hi = xh * yh;

    /* unrolled by the compiler ? */
    for (i = 0; i < 15; i++) {
        mid = (mid << 2) % PRIME;
    }
    mid = (mid << 1) % PRIME;

    /* 2^62 % (2^61 - 1) = 2 */
    hi = (hi << 1) % PRIME;

    return ( (xl*yl + mid + hi) % PRIME );
}

int64_t mulpsmall(int64_t x, int64_t y) {
//Once again we consider integers strictly smaller than the prime
    ( (x*y) % PRIMESMALL );
}

int64_t* matrix_addp(int n, const int64_t *a, const int64_t *b, int64_t *r) {
    int i;
    for (i = 0; i < n*n; i++) {
        r[i] = addp(a[i], b[i]);
    }
    return r;
}

int64_t* matrix_mulp(int n, const int64_t *a, const int64_t *b, int64_t *r) {
    int i, j, k;
    int64_t x;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            x = 0;
            for (k = 0; k < n; k++) {
                x = addp(x, mulp(a[i*n+k], b[k*n+j]));
            }
            r[i*n+j] = x;
        }
    }
    return r;
}

/* copy of size n array, not size n*n matrix */
void copy_int_array_to_int64(int n, const int *src, int64_t *dest) {
    int i;
    for (i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

