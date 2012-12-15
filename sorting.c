#include "sorting.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void insertion_sort_with_keys(int *start, int *end, const int64_t *keys) {
    int tmp;
    int *p, *q;
    for (p = start+1; p < end; p++) {
        q = p;
        tmp = *q;
        while (q != start && keys[tmp] < keys[*(q-1)]) {
            *q = *(q-1);
            q--;
        }
        *q = tmp;
    }
}

struct merge_sort_args {
    int *start;
    int *end;
    const int64_t *keys;
    int *buffer;
    int recursion_depth;
};

void* merge_sort_thread(struct merge_sort_args *args) {
    int *start = args->start;
    int *end = args->end;
    int size = end - start;

    /* Base case : insertion sort is faster on small lists */
    if (size <= 12) {
        insertion_sort_with_keys(start, end, args->keys);
        return NULL;
    }
    
    /* Recursive call */

    struct merge_sort_args args2;

    args2.start = args->end = start + size/2;
    args2.end = end;
    args2.keys = args->keys;
    args2.buffer = args->buffer + size/2;
    args2.recursion_depth = (++(args->recursion_depth));

    /* decide to execute sequentially or create a thread
       depending on the current depth of recursion
       max depth 3 = 8 threads
    */
    if (args->recursion_depth > 3) {
        merge_sort_thread(args);
        merge_sort_thread(&args2);
    } else {
        pthread_t thread;

        pthread_create(&thread, NULL,
                       (void * (*)(void *))merge_sort_thread,
                       &args2);

        merge_sort_thread(args);

        pthread_join(thread, NULL);
    }

    /* Merge ! */
    int i;
    int *p, *q;
    p = start;
    q = start + size/2;
    for (i = 0; i < size; i++) {
        if (args->keys[*p] <= args->keys[*q]) {
            args->buffer[i] = *p;
            p++;
            if (p >= start + size/2) break;
        } else {
            args->buffer[i] = *q;
            q++;
            if (q >= start + size) break;
        }
    }
    i++;
    if (q >= start+size) {
        while (i < size) {
            args->buffer[i++] = *(p++);
        }
    } else {
        while (i < size) {
            args->buffer[i++] = *(q++);
        }
    }
    memcpy(start, args->buffer, size*sizeof(int));

    return NULL;
}


void merge_sort_with_keys(int n, int *array, const int64_t *keys) {
    struct merge_sort_args args;
    args.start = array;
    args.end = array + n;
    args.keys = keys;
    args.recursion_depth = 0;
    args.buffer = malloc(n*sizeof(int)); /* allocate once here,
                                            use for all recursive calls */
    merge_sort_thread(&args);
    free(args.buffer);
}
