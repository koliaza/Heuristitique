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

/* Partition = array of elements + list of boundaries
   format chosen for its suitability in incremental updates
 */

/* add a single partition struct later ? might be of use... */

typedef struct {
    int_list *boundaries;
    int *array_a;
    int *array_b;
} matched_partitions;

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

double possible_matchings_count(int_list *boundaries) {
    double acc = 1.0;
    int_list *p;
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

int compute_equiv_classes_pn(int n, graph_matrix *g_a, graph_matrix *g_b,
                             matched_partitions *partitions) {
    /** Variable initialisation and allocation **/
    int status = 0;

    double cnt = 0;

    int i, j, k;
    const size_t mat64size = n*n*sizeof(int64_t);
    

    /* idea : have separate adjacency matrices for different connected
       components to reduce the size
       --> gain in both space and time at the cost of making the code
           more complicated */
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

        printf("Iteration %d\n", i);

        for(j = 0; j < n; j++) {
            neighbors_a[j] = neighbors_b[j] = 0;
            paths_a[j] = paths_b[j] = 0;
            for (k = 0; k < n; k++) {
                neighbors_a[j] += (a_pow[j*n+k] != 0);
                neighbors_b[j] += (b_pow[j*n+k] != 0);
                paths_a[j] = addp(paths_a[j], a_pow[j*n+k]);
                paths_b[j] = addp(paths_b[j], b_pow[j*n+k]);
            }
        }

        if (!refine_matched_partitions(partitions, neighbors_a, neighbors_b)) {
            printf("incompatible neighbors\n");
            goto compute_equiv_classes_pn_exit;
        }
        if (!refine_matched_partitions(partitions, paths_a, paths_b)) {
            printf("incompatible paths\n");
            goto compute_equiv_classes_pn_exit;
        }
        
        cnt = possible_matchings_count(partitions->boundaries);
        printf("Reduced number of possibilities to %lf\n", cnt);
        if (cnt <= n*n) {
            status = 2;
            goto compute_equiv_classes_pn_exit;
        }

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

int connected_components_test(int n, graph_list *g_a, graph_list *g_b,
                              matched_partitions *partitions) {
    int status = 0;
    int i;

    int *compos_a = malloc(n*sizeof(int));
    int *compos_b = malloc(n*sizeof(int));
    int *compo_sizes_a = malloc(n*sizeof(int));
    int *compo_sizes_b = malloc(n*sizeof(int));
    int64_t *keys_a = malloc(n*sizeof(int64_t));
    int64_t *keys_b = malloc(n*sizeof(int64_t));
    
    gl_connected_components(g_a, compos_a);
    gl_connected_components(g_b, compos_b);

    for (i = 0; i < n; i++) {
        compo_sizes_a[i] = 0;
        compo_sizes_b[i] = 0;
    }
    for (i = 0; i < n; i++) {
        compo_sizes_a[compos_a[i]] += 1;
        compo_sizes_b[compos_b[i]] += 1;
    }
    if (memcmp(compo_sizes_a, compo_sizes_b, n*sizeof(int)) != 0) {
        printf("The number/size of connected components differ\n");
        goto connected_components_test_exit;
    }

    /* not very precise */
    for (i = 0; i < n; i++) {
        keys_a[i] = compo_sizes_a[compos_a[i]];
        keys_b[i] = compo_sizes_b[compos_b[i]];
    }
    status = refine_matched_partitions(partitions, keys_a, keys_b);

connected_components_test_exit:
    free(compos_a);
    free(compos_b);
    free(compo_sizes_a);
    free(compo_sizes_b);
    free(keys_a);
    free(keys_b);

    return status;
}

int find_isomorphism(graph_list *g_a, graph_list *g_b, int *result) {
    int n = g_a->n;
    int i;
    int_list *p;

    matched_partitions partitions;
    init_matched_partitions(n, &partitions);

    /** First tests **/

    assert(n != 0);
    if (n != g_b->n) return 0;

    if (gl_equal(g_a, g_b)) {
        for (i = 0; i < n; i++) {
            result[i] = i;
        }
        return 1;
    }

    if (!connected_components_test(n, g_a, g_b, &partitions))
        return 0;

    /** When the first tests are non-conclusive,
        use the core PN algorithm **/

    graph_matrix *g_a_mat = graph_list_to_matrix(g_a);
    graph_matrix *g_b_mat = graph_list_to_matrix(g_b);
    
    int status = compute_equiv_classes_pn(n, g_a_mat, g_b_mat, &partitions);

    if (status == 0) goto find_isomorphism_exit;

    /** Equivalence classes as (vertex -> number of class) arrays
        allow us to compare easily the classes to which 2 vertices belong
        The following section is used to facilitate both the comparison
        of neighborhoods and the exhaustive search later
     **/
    int c = -1;
    int *a_eqv_cl_array = malloc(n*sizeof(int));
    int *b_eqv_cl_array = malloc(n*sizeof(int));
    int_list **b_eqv_cl_list = malloc(n*sizeof(void*));
    for (i = 0; i < n; i++)
        b_eqv_cl_list[i] = NULL;
    
    for (p = partitions.boundaries; p->next != NULL; p = p->next) {
        c++;
        for (i = p->x; i < p->next->x; i++) {
            a_eqv_cl_array[partitions.array_a[i]] = c;
            b_eqv_cl_array[partitions.array_b[i]] = c;
            b_eqv_cl_list[c] = il_cons(partitions.array_b[i], b_eqv_cl_list[c]);
        }
    }
    c++;

    if (status == 2) goto find_isomorphism_exhaustive;

    /* neighborhood check : do later */

    /** After doing a lot of work to lower the number of possible
        paring of vertices between the 2 graphs, we turn to brute-forcing
        intelligently with a backtracking algorithm **/

find_isomorphism_exhaustive:

    if (result == NULL) {
        int *temp_array = malloc(n*sizeof(int));
        status = pn_exhaustive_search(n, c, a_eqv_cl_array, b_eqv_cl_list,
                                      g_a_mat, g_b_mat, temp_array);
        free(temp_array);
    } else {
        status = pn_exhaustive_search(n, c, a_eqv_cl_array, b_eqv_cl_list,
                                      g_a_mat, g_b_mat, result);
    }

    free(a_eqv_cl_array);
    free(b_eqv_cl_array);
    for (i = 0; i < c; i++) {
        il_free(b_eqv_cl_list[i]);
    }
    free(b_eqv_cl_list);

find_isomorphism_exit:

    free_matched_partitions(&partitions);
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
