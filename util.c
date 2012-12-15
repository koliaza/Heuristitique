#include "util.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

/*** General utilities ***/

double factorial(int n) {
    int i;
    double r = 1.0;
    for (i = 1; i <= n; i++) {
        r *= i;
    }
    return r;
}

/** Big part on sorting **/

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


/*** Linked lists ***/

int_list* il_cons(int x, int_list *l) {
    int_list* r = malloc(sizeof(int_list));
    r->x = x;
    r->next = l;
    return r;
}

int_list* il_singleton(int x) {
    return il_cons(x, NULL);
}

void il_free(int_list *l) {
    int_list *p, *q;
    p = l;
    while (p != NULL) {
        q = p->next;
        free(p);
        p = q;
    }
}

int il_length(const int_list *l) {
    int n = 0;
    for (; l != NULL ; l = l->next) {
        n++;
    }
    return n;
}

int il_equal(const int_list *l1, const int_list *l2) {
    const int_list *p, *q;
    for (p = l1, q = l2; p != NULL && q != NULL; p = p->next, q = q->next) {
        if (p->x != q->x)
            return 0;
    }
    return (p == NULL && q == NULL);
}

/* The insert/remove functions might not have great performance
   because they do a lot of branching */
   
/*** Sorted linked lists ***/

int_list* il_s_insert(int x, int_list *l) {
    int_list *p, *q;

    if (l == NULL || l->x >= x) {
        return il_cons(x, l);
    } else {
        p = l;
        q = l->next;
        while (q != NULL && q->x < x) {
            p = q;
            q = p->next;
        }
        p->next = il_cons(x, q);
        return l;
    }
}

int_list* il_s_insert_once(int x, int_list *l) {
    int_list *p, *q;

    if (l == NULL || l->x > x) {
        return il_cons(x, l);
    } else if (l->x == x)  {
        return l;
    } else {
        p = l;
        q = l->next;
        while (q != NULL && q->x < x) {
            p = q;
            q = p->next;
        }
        if (q->x != x) {
            p->next = il_cons(x, q);
        }
        return l;
    }    
}

int_list* il_s_remove_once(int x, int_list *l) {
    int_list *p, *prev;
    
    if (l == NULL) {
        return NULL;
    } else if (l->x == x) {
        p = l->next;
        free(l);
        return p;
    } else {
        p = l->next; prev = l;
        while (p != NULL && p->x <= x) {
            if (p->x == x) {
                prev->next = p->next;
                return l;
            }
            prev = p;
            p = p->next;
        }
        return l;
    }
}

int_list* il_s_remove_all(int x, int_list *l) {
    int_list *p, *prev;
    
    if (l == NULL) return NULL;

    prev = l; p = l->next;
    while (p != NULL && p->x < x) {
        prev = p;
        p = p->next;
    }
    while (p != NULL && p->x == x) {
        p = p->next;
    }
    prev->next = p;
    return l;
}

int il_s_member(int x, const int_list *l) {
    const int_list *p;
    for (p = l; p != NULL; p = p->next) {
        if (p->x >= x) break;
    }
    return (p != NULL && p->x == x);
}


/*** Matrix manipulation ***/
/* not really actually used because of overflow problems
   see matrix_mod.h for functions better suited to the problem at hand */

int* matrix_make_id(int n, int *r) {
    int i;
    for (i = 0; i < n*n; i++) {
        r[i] = 0;
    }
    for (i = 0; i < n; i++) {
        r[i*n+i] = 1;
    }
    return r;
}

int* matrix_add(int n, const int *a, const int *b, int *r) {
    int i;
    for (i = 0; i < n*n; i++) {
        r[i] = a[i] + b[i];
    }
    return r;
}

int* matrix_mult(int n, const int *a, const int *b, int *r) {
    int i, j, k, x;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            x = 0;
            for (k = 0; k < n; k++) {
                x += a[i*n+k] * b[k*n+j];
            }
            r[i*n+j] = x;
        }
    }
    return r;
}

/* fast exponentiation algorithm, not used anymore but could be useful in a primary test */
int* matrix_exp(int n, const int *a, int m, int *r) {
    /* it would be possible to do one less allocation here
       at the cost of a memmove()
       but it would make the code more complicated */
    int *sq, *tmp;

    if (m == 0) return (matrix_make_id(n, r));
    if (m == 1) return r;

    sq = matrix_mult(n, a, a, malloc(n*n*sizeof(int)));
    if (m % 2 == 0) {
        matrix_exp(n, sq, m/2, r);
    } else {
        tmp = matrix_exp(n, sq, m/2, malloc(n*n*sizeof(int)));
        matrix_mult(n, a, tmp, r);
        free(tmp);
    }
    free(sq);

    return r;
}

/* useful for generating isomorphic graphs */
int* matrix_basis_perm(int n, const int *a, const int *p, int *r) {
    int i,j;
    
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            r[i*n+j] = a[p[i]*n+p[j]];
        }
    }

    return r;
}


/*** Randomness ***/

int coin_flip(double p) {
    return (rand() <= p*RAND_MAX);
}

/* Note : the quality of random number generation is not that important
   for this project */
int random_int_in_range(int a, int b) {
    return floor( ((double) rand() / (((double) RAND_MAX) + 1.0)) * (b - a + 1) + a );
}

/* Using the Knuth-Fisher-Yates algorithm */
void shuffle(int n, int t[]) {
    int i, j, x;
    for (i = 0 ; i < n ; i++) {
        j = random_int_in_range(i, n-1);
        x = t[i];
        t[i] = t[j];
        t[j] = x;
    }
}
