#ifndef PN_HEURISTIC_H
#define PN_HEURISTIC_H

#include <stdint.h>

#include "graph.h"

/* idea : add a hash */
struct pn_entry {
    int vertex;
    int *neighbors;
    int64_t *paths; /* modulo a prime number */
};
typedef struct pn_entry **pn_array;

/* returns a sorted pn_array */
pn_array compute_pn_array(graph_matrix *a);

/* uses backtracking */
int pn_exhaustive_search(int n, int c,
                         int *a_eqv_cl_array, int_list **b_eqv_cl_list,
                         graph_matrix *g_a,   graph_matrix *g_b,
                         int *result);

/* return 0 if the graphs are not isomorphic, 1 if they are
   in the latter case, the result array will be filled with
   a permutation of vertices which defines an isomorphism */
int find_isomorphism(graph_list *a, graph_list *b, int *result);

#endif
