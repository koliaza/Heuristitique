#include "pn_heuristic.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>

#include "graph.h"
#include "matrix_mod.h"
#include "io.h"
#include "partitions.h"

extern int verbose;

/* status info, propagated throughout the search */
enum { NO_ISOM = 0, ISOM_FOUND = 1, SKIP_TO_BRUTEFORCE, NO_STATUS };

/* function prototypes for forward declarations */
int connected_components_test(int, graph_list*, graph_list*,
                              matched_partitions*);
int first_order_test(int, graph_list*, graph_list*, matched_partitions*);
int compute_equiv_classes_pn(int, graph_matrix*, graph_matrix*,
                             matched_partitions*);
int neighborhood_check(int n, graph_list *g_a, graph_list *g_b,
                       int *a_eqv_cl_array, int *b_eqv_cl_array,
                       matched_partitions *partitions);
int pn_exhaustive_search(int, int, int*, int*,
                         graph_matrix*, graph_matrix*, int*);


/*** find_isomorphism() is the function called by external code
     it serves as a wrapper around the different steps of the heuristic
     it manages the global control flow and does all the data conversion
     between the results and expected arguments of the subroutines
***/

/* TODO : cleanup control flow to end the abuse of goto */

int find_isomorphism(graph_list *g_a, graph_list *g_b, int *result) {
    int n = g_a->n;
    int i, status = NO_STATUS;
    double possibilities;

    matched_partitions partitions;

    /** First tests **/

    assert(n != 0);
    if (n != g_b->n) return 0;

    if (gl_equal(g_a, g_b)) {
        for (i = 0; i < n; i++) {
            result[i] = i;
        }
        return 1;
    }

    init_matched_partitions(n, &partitions);

    if (!connected_components_test(n, g_a, g_b, &partitions)) {
        if (verbose)
            printf("The number/size of connected components differ\n");
        free_matched_partitions(&partitions);
        return 0;
    }

    if (!first_order_test(n, g_a, g_b, &partitions)) {
        if (verbose)
            printf("Incompatible list of degrees\n");
        free_matched_partitions(&partitions);
        return 0;
    }

    possibilities = possible_matchings_count(partitions.boundaries);
    if (verbose)
        printf("Number of possibilities remaining after the first tests: %lf\n",
               possibilities);
    if (possibilities <= n*n) { /* arbitrary choice of cutoff */
        status = SKIP_TO_BRUTEFORCE;
    } 

    /** When the first tests are non-conclusive,
        use the core PN algorithm **/

    graph_matrix *g_a_mat = graph_list_to_matrix(g_a);
    graph_matrix *g_b_mat = graph_list_to_matrix(g_b);
    
    if (status != SKIP_TO_BRUTEFORCE) {
        status = compute_equiv_classes_pn(n, g_a_mat, g_b_mat, &partitions);
        if (status == NO_ISOM) goto find_isomorphism_exit;
    }

    /** Equivalence classes as (vertex -> number of class) arrays
        allow us to compare easily the classes to which 2 vertices belong
        The following section is used to facilitate both the comparison
        of neighborhoods and the exhaustive search later
     **/
    int c;
    int *a_eqv_cl_array = malloc(n*sizeof(int));
    int *b_eqv_cl_array = malloc(n*sizeof(int));

    c = tag_arrays_with_matched_partition(&partitions,
                                          a_eqv_cl_array, b_eqv_cl_array);

    /** ... **/

    if (status != SKIP_TO_BRUTEFORCE) {
        status = neighborhood_check(n, g_a, g_b, a_eqv_cl_array, b_eqv_cl_array,
                                    &partitions);
        c = tag_arrays_with_matched_partition(&partitions,
                                              a_eqv_cl_array, b_eqv_cl_array);
        if (verbose)
            printf("Number of possibilities left after the core PN heuristic + neighborhood check : %lf\n", possible_matchings_count(partitions.boundaries));
    }

    /** After doing a lot of work to lower the number of possible
        pairing of vertices between the 2 graphs, we turn to brute-forcing
        intelligently with a backtracking algorithm **/

    if (status != NO_ISOM) {
        if (result == NULL) {
            int *temp_array = malloc(n*sizeof(int));
            status = pn_exhaustive_search(n, c, a_eqv_cl_array, b_eqv_cl_array,
                                          g_a_mat, g_b_mat, temp_array);
            free(temp_array);
        } else {
            status = pn_exhaustive_search(n, c, a_eqv_cl_array, b_eqv_cl_array,
                                          g_a_mat, g_b_mat, result);
        }
    }

    free(a_eqv_cl_array);
    free(b_eqv_cl_array);

find_isomorphism_exit:

    free_matched_partitions(&partitions);
    gm_free(g_a_mat);
    gm_free(g_b_mat);

    return (status != NO_ISOM);
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

int first_order_test(int n, graph_list *g_a, graph_list *g_b,
                     matched_partitions *partitions) {
    int64_t *degrees_a = malloc(n*sizeof(int64_t));
    int64_t *degrees_b = malloc(n*sizeof(int64_t));
    int i, status;
    for (i = 0; i < n; i++) {
        degrees_a[i] = gl_vertex_degree(g_a, i);
        degrees_b[i] = gl_vertex_degree(g_b, i);
    }
    status = refine_matched_partitions(partitions, degrees_a, degrees_b);
    free(degrees_a);
    free(degrees_b);
    return status;
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
    
    /* since we should already have discriminated on the degrees at this step,
       we can start at the 2nd power */
    copy_int_array_to_int64(n*n, g_a->matrix, a64);
    matrix_mulpsmall(n, a64, a64, a_pow);
    copy_int_array_to_int64(n*n, g_b->matrix, b64);
    matrix_mulpsmall(n, b64, b64, b_pow);

    /** Main loop **/
    /* invariant : A_pow = A^(i+1) */
    for (i = 1; i < n; i++) {

        if (verbose)
            printf("PN heuristic : iteration %d\n", i);

        for(j = 0; j < n; j++) {
            neighbors_a[j] = neighbors_b[j] = 0;
            paths_a[j] = paths_b[j] = 0;
            for (k = 0; k < n; k++) {
                neighbors_a[j] += (a_pow[j*n+k] != 0);
                neighbors_b[j] += (b_pow[j*n+k] != 0);
                paths_a[j] = addpsmall(paths_a[j], a_pow[j*n+k]);
                paths_b[j] = addpsmall(paths_b[j], b_pow[j*n+k]);
            }
        }

        if (!refine_matched_partitions(partitions, neighbors_a, neighbors_b)) {
            if (verbose)
                printf("incompatible neighbors\n");
            goto compute_equiv_classes_pn_exit;
        }
        if (!refine_matched_partitions(partitions, paths_a, paths_b)) {
            if (verbose)
                printf("incompatible paths\n");
            goto compute_equiv_classes_pn_exit;
        }
        
        cnt = possible_matchings_count(partitions->boundaries);
        if (verbose)
            printf("Reduced number of possibilities to %lf\n", cnt);
        if (cnt <= n*n) {
            status = 2;
            goto compute_equiv_classes_pn_exit;
        }

        /* update for next iteration */
        matrix_mulpsmall(n, a64, a_pow, tmp);
        memcpy(a_pow, tmp, mat64size);
        matrix_mulpsmall(n, b64, b_pow, tmp);
        memcpy(b_pow, tmp, mat64size);
    }
    
    status = 1;

compute_equiv_classes_pn_exit:
    free(a64);
    free(a_pow);
    free(tmp);

    return status;
}

int neighborhood_check(int n, graph_list *g_a, graph_list *g_b,
                       int *a_eqv_cl_array, int *b_eqv_cl_array,
                       matched_partitions *partitions) {
    int status = NO_STATUS;
    int i,j;
    int_list *p;
    
    int64_t *a_num_neighbors_in_class = malloc(n*sizeof(int64_t));
    int64_t *b_num_neighbors_in_class = malloc(n*sizeof(int64_t));

    /* repeat n times for theoretical reasons, could be optimized to stop when it cycles */
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            a_num_neighbors_in_class[j] = 0;
            for (p = g_a->list_array[j]; p != NULL; p = p->next) {
                if (a_eqv_cl_array[p->x] == i)
                    a_num_neighbors_in_class[j] += 1;
            } 
            b_num_neighbors_in_class[j] = 0;
            for (p = g_b->list_array[j]; p != NULL; p = p->next) {
                if (b_eqv_cl_array[p->x] == i)
                    b_num_neighbors_in_class[j] += 1;
            }
        }
        refine_matched_partitions(partitions,
                                  a_num_neighbors_in_class,
                                  b_num_neighbors_in_class);
        tag_arrays_with_matched_partition(partitions,
                                          a_eqv_cl_array,
                                          b_eqv_cl_array);
    }

    return status;
}


struct choice_stack {
    int a_vertex;
    int b_choice;
    int_list *b_untested;
    struct choice_stack *rest;
};

int pn_exhaustive_search(int n, int c,
                         int *a_eqv_cl_array, int *b_eqv_cl_array,
                         graph_matrix *g_a,   graph_matrix *g_b,
                         int *result) {
    int status = NO_STATUS;
    /* the result array is used to store the potential partial isomorphisms */
    int v_a = -1;
    int i;
    int partial_ok = 1;
    struct choice_stack *cs = NULL;
    struct choice_stack *tmp = NULL;

    int_list **b_eqv_cl_list = malloc(c*sizeof(void*));
    for (i = 0; i < c; i++) 
        b_eqv_cl_list[i] = NULL;
    for (i = 0; i < n; i++) {
        b_eqv_cl_list[b_eqv_cl_array[i]] =
            il_cons(i, b_eqv_cl_list[b_eqv_cl_array[i]]);
    }

    for (;;) {
        /* if everything goes well, try one more vertex */
        if (partial_ok) {
            v_a++;
            
            if (verbose) {
                printf("Vertex %d of A : possibilities in B are ", v_a);
                print_list(b_eqv_cl_list[a_eqv_cl_array[v_a]]);
                putchar('\n');
            }

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
            if (cs == NULL) {
                status = NO_ISOM;
                goto pn_exhaustive_search_exit;
            }
            v_a = cs->a_vertex;
        }
        /* try next possibility */
        result[v_a] = cs->b_untested->x;
        cs->b_untested = cs->b_untested->next;
        if (verbose)
            printf("Testing A[%d] -> B[%d]\n", v_a, result[v_a]);
        
        partial_ok = 1;
        for (i = 0; i < v_a; i++) {
            if (gm_edge(g_a, i, v_a) != gm_edge(g_b, result[i], result[v_a])) {
                partial_ok = 0;
                break;
            }
        }

        if (v_a == n-1 && partial_ok) {
            status = ISOM_FOUND;
            break;
        }
    }

pn_exhaustive_search_exit:
        
    for (i = 0; i < c; i++) {
        il_free(b_eqv_cl_list[i]);
    }
    free(b_eqv_cl_list);

    return status;
}


/*******************************************************/

int compare_pn_entries_aux(const struct pn_entry *e1,
                           const struct pn_entry *e2,
                           const int pn_entry_vect_size) {
    int x = memcmp(e1->neighbors, e2->neighbors,
                   pn_entry_vect_size * sizeof(int));
    if (x != 0) return x;
    else return memcmp(e1->paths, e2->paths,
                       pn_entry_vect_size * sizeof(int64_t));
}
int compare_pn_entries(const void *e1, const void *e2, void *arg) {
    return compare_pn_entries_aux(*((struct pn_entry**)e1),
                                  *((struct pn_entry**)e2),
                                  *((int*)arg));
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

    qsort_r(pn_array, n, sizeof(void*), compare_pn_entries, &n);
    return pn_array;
}

/****************************************************************/

/*** On a set of multiple graphs ***/

struct process_graph_args {
    graph_list *graph_list_arg;
    graph_matrix **matrix_dest;
    pn_array *pn_array_dest;
    int *class_num_dest;
    int **tagged_array_dest;
};

void* process_graph(struct process_graph_args *args) {
    graph_matrix *g_m = *(args->matrix_dest)
                      = graph_list_to_matrix(args->graph_list_arg);
    pn_array pn = *(args->pn_array_dest) = compute_pn_array(g_m);
    
    int i;
    int c = 0; /* c : index of current equiv class */
    int *arr = *(args->tagged_array_dest) = malloc(g_m->n * sizeof(int));
    int *cur_neighbors = pn[0]->neighbors;
    int64_t *cur_paths = pn[0]->paths;
    arr[pn[0]->vertex] = 0;
    for (i = 1; i < g_m->n; i++) {
        /* these comparisons could be improved with hashes */
        if (memcmp(pn[i]->neighbors, cur_neighbors, g_m->n*sizeof(int)) != 0
            || memcmp(pn[i]->paths, cur_paths, g_m->n*sizeof(int64_t)) != 0)
        {
            c++;
            cur_neighbors = pn[i]->neighbors;
            cur_paths     = pn[i]->paths;
        }
        arr[pn[i]->vertex] = c;
    }
    *args->class_num_dest = c + 1;

    printf("done\n");
    
    return NULL;
}

void find_multiple_isomorphisms(int graph_count, graph_list **graphs,
                                int *isom_classes) {
    int i, j, k;
    struct process_graph_args args;
    graph_matrix **matrices = malloc(graph_count*sizeof(void*));
    pn_array *pn_arrays = malloc(graph_count*sizeof(void*));
    pthread_t *threads = malloc(graph_count*sizeof(pthread_t));
    int *class_nums = malloc(graph_count*sizeof(int));
    int **tagged_arrays = malloc(graph_count*sizeof(void*));
    
    for (i = 0; i < graph_count; i++) {
        args.graph_list_arg = graphs[i];
        args.matrix_dest = matrices + i;
        args.pn_array_dest = pn_arrays + i;
        args.class_num_dest = class_nums + i;
        args.tagged_array_dest = tagged_arrays + i;
        pthread_create(threads + i, NULL,
                       (void * (*)(void *))process_graph, &args);
    }
    for (i = 0; i < graph_count; i++) {
        pthread_join(threads[i], NULL);
    }

    int64_t *keys = malloc(graph_count*sizeof(int64_t));
    partition part;
    init_partition(graph_count, &part);

    for (i = 0; i < graph_count; i++) {
        keys[i] = matrices[i]->n;
    }
    refine_partition(&part, keys);

    int_list *size_boundaries = il_copy(part.boundaries);

    /* the following code is a complicated way
       to refine separately different classes of a partition
       and then update that partition to reflect the changes */
    int_list *p, *q, *ppp;
    q = part.boundaries;
    for (p = size_boundaries; p->next != NULL; p = p->next) {
        int n = matrices[p->x]->n;
        int subclass_count = p->next->x - p->x;
        partition subpart;
        init_partition(subclass_count, &subpart);
        for (i = p->x; i < p->next->x; i++) {
            subpart.array[i - p->x] = part.array[i];
        }
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                for (k = 0; k < subclass_count; k++) {
                    printf("%p\n", pn_arrays[p->x + k][i]->neighbors);
                    keys[k] = pn_arrays[p->x + k][i]->neighbors[j];
                }
                refine_partition(&subpart, keys);
            }
        }
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                for (k = 0; k < subclass_count; k++) {
                    keys[k] = pn_arrays[p->x + k][i]->paths[j];
                }
                refine_partition(&subpart, keys);
            }
        }
        for (ppp = subpart.boundaries; ppp != NULL; ppp = ppp->next) {
            ppp->x += p->x;
        }
        for (i = p->x; i < p->next->x; i++) {
            part.array[i] = subpart.array[i - p->x];
        }
        ppp = q->next;
        q->next = il_append(subpart.boundaries->next, q->next->next);
        while (q->x != ppp->x)
            q = q->next;
        free(ppp);
        free_partition(&subpart);
    }

    int start, end;
    int *useless_result;
    for (p = part.boundaries; p->next != NULL; p = p->next) {
        start = p->x;
        end = p->next->x;
        i = start;
        while (i < end) {
            k = i+1;
            for (j = i+1; j < end; j++) {
                int graph_i = part.array[i];
                int graph_j = part.array[j];
                useless_result = malloc(matrices[graph_i]->n * sizeof(int));
                if (pn_exhaustive_search(matrices[graph_i]->n,
                                         class_nums[graph_i],
                                         tagged_arrays[graph_i],
                                         tagged_arrays[graph_j],
                                         matrices[graph_i],
                                         matrices[graph_j],
                                         useless_result)) {
                    part.array[j] = part.array[k];
                    part.array[k] = graph_j;
                    k++;
                }
                i = k;
                p->next = il_cons(k, p->next);
                p = p->next;
            }
        }
    }
    
    tag_array_with_partition(&part, isom_classes);

    free_partition(&part);
    for (i = 0; i < graph_count; i++) {
        gm_free(matrices[i]);
        for (j = 0; j < matrices[i]->n; j++) {
            free(pn_arrays[i][j]->neighbors);
            free(pn_arrays[i][j]->paths);
            free(pn_arrays[i][j]);
        }
        free(pn_arrays[i]);
        free(tagged_arrays[i]);
    }
    free(matrices);
    free(pn_arrays);
    free(threads);
    free(class_nums);
    free(tagged_arrays);
}

