#include "pn_heuristic.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* for debug */
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "graph.h"
#include "matrix_mod.h"
#include "io.h"

/*** Partitions / Equivalence classes on the vertices of a graph ***/

/* Partition = array of elements + list of boundaries */

/* TODO : rewrite to eliminate the need to use the non-standard
   qsort_r function */

int compare_with_keys(const void *x, const void *y, void *keys) {
    int64_t foo = ((int64_t*)keys)[*((int*)y)] - ((int64_t*)keys)[*((int*)x)];
    if (foo == 0) return 0;
    else if (foo < 0) return (-1);
    else return 1;
}

int refine_matching_partitions(int_list *boundaries,
                               int *array_a,    int *array_b,
                               int64_t *keys_a, int64_t *keys_b) {
    int_list *p, *q;
    int start, end;
    int i;
    int64_t cur_key, new_key;

    for (p = boundaries; p->next != NULL; p = p->next) {
        start = p->x;
        end = p->next->x; /* memory location after the end */  
        
        qsort_r(array_a + start, end-start, sizeof(int),
                compare_with_keys, keys_a);
        qsort_r(array_b + start, end-start, sizeof(int),
                compare_with_keys, keys_b);
        
        cur_key = keys_a[array_a[start]];
        if (cur_key != keys_b[array_b[start]]) {
            printf("foo\n");
            return 0;
        }
        q = p;
        for (i = start+1; i < end; i++) {
            new_key = keys_a[array_a[i]];
            if (new_key != keys_b[array_b[i]]) {
                printf("foobar\n");
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

int compute_equiv_classes_pn(int n, graph_matrix *g_a, graph_matrix *g_b,
                             int_list *boundaries,
                             int *partition_array_a, int *partition_array_b) {
    /** Variable initialisation and allocation **/
    int status = 0;

    int i, j, k;
    const size_t mat64size = n*n*sizeof(int64_t);

    int64_t *tmp = malloc(mat64size);
    int64_t *a64 = malloc(mat64size);
    int64_t *a_pow = malloc(mat64size);
    int64_t *b64 = malloc(mat64size);
    int64_t *b_pow = malloc(mat64size);

    /* all keys for sorting are int64 to make things a bit easier */
    int64_t *neighbors_a = malloc(n*sizeof(int64_t));
    int64_t *paths_a = malloc(n*sizeof(int64_t));
    int64_t *neighbors_b = malloc(n*sizeof(int64_t));
    int64_t *paths_b = malloc(n*sizeof(int64_t));
    
    copy_int_array_to_int64(n*n, g_a->matrix, a64);
    memcpy(a_pow, a64, mat64size);
    copy_int_array_to_int64(n*n, g_b->matrix, b64);
    memcpy(b_pow, b64, mat64size);

    /** Main loop **/
    /* invariant : A_pow = A^(i+1) */
    for (i = 0; i < n; i++) {

        for(j = 0; j < n; j++) {
            neighbors_a[j] = neighbors_b[j] = 0;
            paths_a[i] = paths_b[i] = 0;
            for (k = 0; k < n; k++) {
                neighbors_a[j] += (a_pow[j*n+k] != 0);
                neighbors_b[j] += (b_pow[j*n+k] != 0);
                paths_a[j] = addp(paths_a[j], a_pow[j*n+k]);
                paths_b[j] = addp(paths_b[j], b_pow[j*n+k]);
            }
        }

        if (!refine_matching_partitions(boundaries,
                                        partition_array_a, partition_array_b,
                                        neighbors_a, neighbors_b)) {
            printf("incompatible neighbors\n");
            goto compute_equiv_classes_pn_exit;
        }
        if (!refine_matching_partitions(boundaries,
                                        partition_array_a, partition_array_b,
                                        paths_a, paths_b)) {
            printf("incompatible paths\n");
            goto compute_equiv_classes_pn_exit;
        }

        /* maybe compute the number of possibilities to return quickly
           with special exit code and enter directly the backtracking
         */

        /* update for next iteration */
        matrix_mulp(n, a64, a_pow, tmp);
        memcpy(a_pow, tmp, mat64size);
        matrix_mulp(n, b64, b_pow, tmp);
        memcpy(b_pow, tmp, mat64size);
    }
    
    status = 1;

compute_equiv_classes_pn_exit:
    free(a64);
    free(a_pow);
    free(tmp);

    return status;
}


int find_isomorphism(graph_list *g_a, graph_list *g_b, int *result) {
    int n = g_a->n;
    int i;
    int_list *p;

    int_list *boundaries = il_cons(0, il_singleton(n));
    int *partition_array_a = malloc(n*sizeof(int));
    int *partition_array_b = malloc(n*sizeof(int));

    for (i = 0; i < n; i++) {
        partition_array_a[i] = i;
        partition_array_b[i] = i;
    }

    /** First tests **/

    assert(n != 0);
    if (n != g_b->n) return 0;
/*
    if (gl_equal(g_a, g_b)) {
        for (i = 0; i < n; i++) {
            result[i] = i;
        }
        return 1;
    }
*/ 
    /** When the first tests are non-conclusive,
        use the core PN algorithm **/

    graph_matrix *g_a_mat = graph_list_to_matrix(g_a);
    graph_matrix *g_b_mat = graph_list_to_matrix(g_b);
    
    int status = compute_equiv_classes_pn(n, g_a_mat, g_b_mat, boundaries,
                                          partition_array_a,
                                          partition_array_b);
    printf("foo\n");

    if (status == 0) goto find_isomorphism_exit;

    /* check for neighbors later
       it's gonna be ugly ... */
    
    /* equivalence class representation */
    int c = -1;
    int *a_eqv_cl_array = malloc(n*sizeof(int));
    int_list **b_eqv_cl_list = malloc(n*sizeof(int));
    for (i = 0; i < n; i++)
        b_eqv_cl_list[i] = NULL;
    
    print_list(boundaries);

    for (p = boundaries; p->next != NULL; p = p->next) {
        c++;
        for (i = p->x; i < p->next->x; i++) {
            a_eqv_cl_array[partition_array_a[i]] = c;
            b_eqv_cl_list[c] = il_cons(partition_array_b[i], b_eqv_cl_list[c]);
        }
    }
    c++;

    if (result == NULL) {
        int *temp_array = malloc(n*sizeof(int));
        status = pn_exhaustive_search(n, c, a_eqv_cl_array,
                                      b_eqv_cl_list, g_a_mat, g_b_mat,
                                      temp_array);
        free(temp_array);
    } else {
        status = pn_exhaustive_search(n, c, a_eqv_cl_array,
                                      b_eqv_cl_list, g_a_mat, g_b_mat,
                                      result);
    }
    free(a_eqv_cl_array);
    for (i = 0; i < c; i++) {
        il_free(b_eqv_cl_list[i]);
    }
    free(b_eqv_cl_list);

find_isomorphism_exit:
    il_free(boundaries);
    free(partition_array_a);
    free(partition_array_b);
    gm_free(g_a_mat);
    gm_free(g_b_mat);
    return status;
}


/** this is old
    will be eliminated when the qsort_r replacement comes **/

/* global var as implicit parameter
   to compensate the lack of closures
   IMPORTANT : it means you cannot run 2 comparisons
   with different values of n in parallel
*/
static int pn_entry_vect_size;
int compare_pn_entries_aux(const struct pn_entry *e1,
                           const struct pn_entry *e2) {
    int x = memcmp(e1->neighbors, e2->neighbors,
                   pn_entry_vect_size * sizeof(int));
    if (x != 0) return x;
    else return memcmp(e1->paths, e2->paths,
                       pn_entry_vect_size * sizeof(int64_t));
}
int compare_pn_entries(const void *e1, const void *e2) {
    return compare_pn_entries_aux(*((struct pn_entry**)e1),
                                  *((struct pn_entry**)e2));
}



typedef int64_t *p_matrix;
typedef int *n_matrix;

pn_array compute_pn_array(graph_matrix *g) {
    /** Variable initialisation and allocation **/
    pn_array pn_array;

    int i, j, k, n = g->n;
    p_matrix p_mat = malloc(n*n*sizeof(*p_mat));
    n_matrix n_mat = malloc(n*n*sizeof(*n_mat));
    const size_t mat64size = n*n*sizeof(int64_t);

    int64_t *tmp = malloc(mat64size);
    int64_t *a64 = malloc(mat64size);
    int64_t *a_pow = malloc(mat64size);
    copy_int_array_to_int64(n*n, g->matrix, a64);
    memcpy(a_pow, a64, mat64size);

    /** Initially fill the matrix **/

    /* invariant : A_pow = A^(i+1) */
    for (i = 0; i < n; i++) {
        for(j = 0; j < n; j++) {
            n_mat[i*n+j] = 0; p_mat[i*n+j] = 0;
            for (k = 0; k < n; k++) {
                n_mat[i*n+j] += (a_pow[j*n+k] != 0);
                p_mat[i*n+j] = addp(p_mat[i*n+j], a_pow[j*n+k]);
            }
        }
        /* update for next iteration */
        matrix_mulp(n, a64, a_pow, tmp);
        memcpy(a_pow, tmp, mat64size);
    }
    /* we won't need these anymore... */
    free(a64);
    free(a_pow);
    free(tmp);

    /** Build the pn_array structure **/
    
    pn_array = malloc(n*sizeof(struct pn_entry));
    for (i = 0; i < n; i++) {
        pn_array[i] = malloc(sizeof(struct pn_entry));
        pn_array[i]->vertex = i;
        pn_array[i]->neighbors = malloc(n*sizeof(int));
        pn_array[i]->paths = malloc(n*sizeof(int64_t));
        for (j = 0; j < n; j++) {
            /* deliberate transposition ! */
            pn_array[i]->neighbors[j] = n_mat[j*n+i];
            pn_array[i]->paths[j]     = p_mat[j*n+i];
        }
    }
    free(n_mat);
    free(p_mat);

    /** One more final **/

    pn_entry_vect_size = n;
    qsort(pn_array, n, sizeof(void*), compare_pn_entries);
    return pn_array;
}

struct choice_stack {
    int a_vertex;
    int b_choice;
    int_list *b_untested;
    struct choice_stack *rest;
};

int pn_exhaustive_search(int n, int c,
                         int *a_eqv_cl_array, int_list **b_eqv_cl_list,
                         graph_matrix *g_a,   graph_matrix *g_b,
                         int *result) {
    /* the result array is used to store the tentative partial isomorphisms */
    int v_a = -1;
    int i;
    int partial_ok = 1;
    struct choice_stack *cs = NULL;
    struct choice_stack *tmp = NULL;
    for (;;) {
        /* if everything goes well, try one more vertex */
        if (partial_ok) {
            v_a++;
            
            printf("Vertex %d of A : possibilities in B are ", v_a);
            print_list(b_eqv_cl_list[a_eqv_cl_array[v_a]]);
            putchar('\n');

            /* stack push */
            tmp = cs;
            cs = malloc(sizeof(struct choice_stack));
            cs->a_vertex = v_a;
            cs->b_untested = b_eqv_cl_list[a_eqv_cl_array[v_a]];
            cs->rest = tmp;
        }
        /* backtrack in case of failure */
        while (cs->b_untested == NULL) {
            tmp = cs;
            cs = cs->rest;
            v_a = cs->a_vertex;
            free(tmp);
            if (cs == NULL) return 0;
        }
        /* try next possibility */
        result[v_a] = cs->b_untested->x;
        cs->b_untested = cs->b_untested->next;
        printf("Testing A[%d] -> B[%d]\n", v_a, result[v_a]);
        
        partial_ok = 1;
        for (i = 0; i < v_a; i++) {
            if (gm_edge(g_a, i, v_a) != gm_edge(g_b, result[i], result[v_a])) {
                partial_ok = 0;
                break;
            }
        }

        if (v_a == n-1 && partial_ok) return 1;
    }
}
