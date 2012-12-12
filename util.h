#ifndef UTIL_H
#define UTIL_H

/*** Linked lists of integers ***/

typedef struct int_list int_list;
struct int_list {
    int x;
    int_list *next;
};

int_list* il_cons(int x, int_list *l);
int_list* il_singleton(int x);
/* recursively free all elements */
void il_free(int_list *l);
int il_length(const int_list *l);

/* These functions only work correctly on __sorted__ lists,
   and they are __destructive__ */
int_list* il_s_insert(int x, int_list *l);
/* maybe call this insert_if_absent */
int_list* il_s_insert_once(int x, int_list *l);
int_list* il_s_remove_once(int x, int_list *l);
/* int_list* il_s_remove_all(int x, int_list *l); */
int il_s_member(int x, const int_list *l);


/*** Matrix manipulation ***/

/* these functions operate on 32 bits ints for now; there's a risk of overflow
   TODO (maybe ?) : convert everything to int64
   they all take as their last parameter a pointer to the destination where
   the result of the computation will be written
*/

int* matrix_make_id(int n, int *r);

int* matrix_add(int n, const int *a, const int *b, int *r);

/* Naive algorithm, runs in O(n^3) */
int* matrix_mult(int n, const int *a, const int *b, int *r);

/* Fast exponentiation in O(n^3 log m) */
int* matrix_exp(int n, const int *a, int m, int *r);

/* a : matrix, p : permutation of [1..n]
   computes P^{-1}AP
   runs in O(n^2)
   TODO : write matrix_basis_transpose which will run in O(n) */
int* matrix_basis_perm(int n, const int* a, const int* p, int *r);

/*** Randomness ***/

int coin_flip(double p);
int random_int_in_range(int a, int b);
void shuffle(int n, int t[]);

#endif
