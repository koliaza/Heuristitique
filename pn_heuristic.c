#include "pn_heuristic.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "graph.h"
#include "matrix_mod.h"

int pn_heuristic(int n, graph_matrix *g_a, graph_matrix *g_b, int *result);

int find_isomorphism(graph_list *g_a, graph_list *g_b, int *result) {
    int n;
    int i;

    /** First tests **/

    n = g_a->n;
    assert(n != 0);
    if (n != g_b->n) return 0;

    if (gl_equal(g_a, g_b)) {
        for (i = 0; i < n; i++) {
            result[i] = i;
        }
        return 1;
    }

    /** When the first tests are non-conclusive,
        use the core PN algorithm **/

    graph_matrix *g_a_mat = graph_list_to_matrix(g_a);
    graph_matrix *g_b_mat = graph_list_to_matrix(g_b);
    
    int status = pn_heuristic(n, g_a_mat, g_b_mat, result);

    gm_free(g_a_mat);
    gm_free(g_b_mat);

    return status;
}

int pn_heuristic(int n, graph_matrix *g_a, graph_matrix *g_b, int *result) {
    int i, c;
    pn_array pn_a, pn_b;
    int *cur_neighbors = NULL;
    int64_t *cur_paths = NULL;
    int *a_eqv_cl_array;
    int_list **b_eqv_cl_list;
    
    int status = 0;

    /** Comparison of PN-arrays and partitioning into equivalence classes **/

    pn_a = compute_pn_array(g_a);
    pn_b = compute_pn_array(g_b);

    a_eqv_cl_array = malloc(n*sizeof(int));
    b_eqv_cl_list = malloc(n*sizeof(int_list*));
    for (i = 0; i < n; i++) {
        a_eqv_cl_array[i] = 0;
        b_eqv_cl_list[i] = NULL;
    }
 
    c = 0; /* c : index of current equiv class */
    a_eqv_cl_array[pn_a[0]->vertex] = 0;
    b_eqv_cl_list[0] = il_singleton(pn_b[0]->vertex);
    cur_neighbors = pn_a[0]->neighbors;
    cur_paths     = pn_a[0]->paths;
    if (memcmp(cur_neighbors, pn_b[0]->neighbors, n*sizeof(int)) != 0
        || memcmp(cur_paths, pn_b[0]->paths, n*sizeof(int64_t)) != 0)
        goto pn_heuristic_exit;

    /* these comparisons could be improved with hashes */
    for (i = 1; i < n; i++) {
        int *an = pn_a[i]->neighbors;
        int64_t *ap = pn_a[i]->paths;
        int *bn = pn_b[i]->neighbors;
        int64_t *bp = pn_b[i]->paths;

        if (memcmp(an, bn, n*sizeof(int)) != 0
            || memcmp(ap, bp, n*sizeof(int64_t)) != 0)
            goto pn_heuristic_exit;

        if (memcmp(an, cur_neighbors, n*sizeof(int)) != 0
            || memcmp(ap, cur_paths, n*sizeof(int64_t)) != 0)
        {
            c++;
            cur_neighbors = an;
            cur_paths     = ap;
        }
        a_eqv_cl_array[pn_a[i]->vertex] = c;
        b_eqv_cl_list[c] = il_cons(pn_b[i]->vertex, b_eqv_cl_list[c]);
    }
    c++; /* c = number of equivalence classes */


    /** once the equivalence classes are computed,
        run an exhaustive search on the limited number
        of remaining possibilities for isomorphism **/
    if (result == NULL) {
        int *temp_array = malloc(n*sizeof(int));
        status = pn_exhaustive_search(n, c, a_eqv_cl_array,
                                      b_eqv_cl_list, g_a, g_b,
                                      temp_array);
        free(temp_array);
    } else {
        status = pn_exhaustive_search(n, c, a_eqv_cl_array,
                                      b_eqv_cl_list, g_a, g_b,
                                      result);
    }

    /** Cleanup **/

pn_heuristic_exit:
    
    for (i = 0; i < c; i++) {
        il_free(b_eqv_cl_list[i]);
    }
    free(a_eqv_cl_array);
    free(b_eqv_cl_list);
    for (i = 0; i < n; i++) {
        free(pn_a[i]);
        free(pn_b[i]);
    }
    free(pn_a);
    free(pn_b);
    
    return status;
}


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
            free(tmp);
            if (cs == NULL) return 0;
        }
        /* try next possibility */
        result[v_a] = cs->b_untested->x;
        cs->b_untested = cs->b_untested->next;
        
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
