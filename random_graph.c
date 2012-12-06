#include "random_graph.h"

#include "graph.h"
#include "util.h"

#include <stdlib.h>

graph_matrix* erdos_renyi_gnp(int n, double p) {
    int i, j;
    graph_matrix* g;

    g = gm_alloc(n);
    for (i = 0; i < n; i++) {
        g->matrix[i*n+i] = 0;
        for (j = i+1; j < n; j++) {
            if (coin_flip(p))
                gm_edge_add(g,i,j);
            else
                gm_edge_remove(g,i,j);
        }
    }
    
    return g;
}

graph_matrix* erdos_renyi_gnm(int n, int m) {
    int i;
    graph_matrix* g;

    g = gm_init_zero(gm_alloc(n));
    for (i = 0; i < m; i++) {
        gm_multi_edge_add(g,
                          random_int_in_range(0, n-1),
                          random_int_in_range(0, n-1));
    }
    
    return g;
}

graph_matrix* random_k_regular(int n, int k) {
    int i, j;
    int *t;
    graph_matrix* g;

    t = malloc(n*sizeof(int));
    for (i = 0; i < n; i++)
        t[i] = i;

    g = gm_init_zero(gm_alloc(n));

    for (i = 0; i < k; i++) {
        shuffle(n, t);
        for (j = 0; j < n; j++) {
            if (gm_edge(g, i, t[i])) break;
        }
        if (j < n) { i--; continue; }
        
        for (j = 0; j < n; j++)
            gm_edge_add(g, j, t[j]);
            
    }

    free(t);
    return g;
}

graph_matrix* random_k_regular_multi(int n, int k) {
    int i, j;
    int *t;
    graph_matrix* g;

    t = malloc(n*sizeof(int));
    for (i = 0; i < n; i++)
        t[i] = i;

    g = gm_init_zero(gm_alloc(n));

    for (i = 0; i < k; i++) {
        shuffle(n, t);
        for (j = 0; j < n; j++)
            gm_multi_edge_add(g, j, t[j]);
    }

    free(t);
    return g;
}
