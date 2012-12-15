#include "partitions.h"

#include <stdio.h>
#include <stdlib.h>

matched_partitions* init_matched_partitions(int n, matched_partitions *pp) {
    int i;
    pp->boundaries = il_cons(0, il_singleton(n));
    pp->array_a = malloc(n*sizeof(int));
    pp->array_b = malloc(n*sizeof(int));
    for (i = 0; i < n; i++) {
        pp->array_a[i] = i;
        pp->array_b[i] = i;
    }
    return pp;
}

void free_matched_partitions(matched_partitions *pp) {
    il_free(pp->boundaries);
    free(pp->array_a);
    free(pp->array_b);
}

double possible_matchings_count(const int_list *boundaries) {
    double acc = 1.0;
    const int_list *p;
    for (p = boundaries; p->next != NULL; p = p->next) {
        acc *= factorial(p->next->x - p->x);
    }
    return acc;
}

/* TODO : rewrite to eliminate the need to use the non-standard
   qsort_r function */

int compare_with_keys(const void *x, const void *y, void *keys) {
    int64_t foo = ((int64_t*)keys)[*((int*)y)] - ((int64_t*)keys)[*((int*)x)];
    if (foo == 0) return 0;
    else if (foo < 0) return (-1);
    else return 1;
}

int refine_matched_partitions(matched_partitions *pp,
                              const int64_t *keys_a, const int64_t *keys_b) {
    int_list *p, *q;
    int start, end;
    int i;
    int64_t cur_key, new_key;

    for (p = pp->boundaries; p->next != NULL; p = p->next) {
        start = p->x;
        end = p->next->x; /* memory location after the end */  
        
        qsort_r(pp->array_a + start, end-start, sizeof(int),
                compare_with_keys, keys_a);
        qsort_r(pp->array_b + start, end-start, sizeof(int),
                compare_with_keys, keys_b);
        
        cur_key = keys_a[pp->array_a[start]];
        if (cur_key != keys_b[pp->array_b[start]]) {
            return 0;
        }
        q = p;
        for (i = start+1; i < end; i++) {
            new_key = keys_a[pp->array_a[i]];
            if (new_key != keys_b[pp->array_b[i]]) {
                return 0;
            }
            if (new_key != cur_key) {
                cur_key = new_key;
                q->next = il_cons(i, q->next);
                q = q->next;
            }
        }
    }
    
    return 1;
}

/* return value : number of subsets in partition */
int tag_arrays_with_matched_partition(matched_partitions *pp,
                                      int *a_array, int *b_array) {
    int c = -1;
    int i;
    int_list *p;
    for (p = pp->boundaries; p->next != NULL; p = p->next) {
        c++;
        for (i = p->x; i < p->next->x; i++) {
            a_array[pp->array_a[i]] = c;
            b_array[pp->array_b[i]] = c;
        }
    }
    return (c+1);
}
