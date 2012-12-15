#ifndef SORTING_H
#define SORTING_H

#include <stdint.h>

/* replacement for the non-standard qsort_r function */
void merge_sort_with_keys(int n, int *array, const int64_t *keys);

#endif
