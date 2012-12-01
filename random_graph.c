#include "random_graph.h"

#include "graph.h"
#include "util.h"

graph_matrix* erdos_renyi(int n, double p) {
    int i, j;
    graph_matrix* g;

    g = alloc_graph_matrix(n);
    for (i = 0; i < n; i++) {
        g->matrix[i*n+i] = 0;
        for (j = i+1; j < n; j++) {
            if (coin_flip(p))
                gm_edge_add(i,j);
            else
                gm_edge_remove(i,j);
            }
        }
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

    g = alloc_graph_matrix(n);
    for (i = 0; i < n*n; i++)
        g->matrix[i] = 0;

    for (i = 0; i < k; i++) {
        shuffle(n, t);
        for (j = 0; j < n; j++) {
            if gm_edge(g, i, t[i]) break;
        }
        if (j < n) { i--; continue; }
        
        for (j = 0; j < n; j++)
            gm_edge_add(j, t[j]);
            
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

    g = alloc_graph_matrix(n);
    for (i = 0; i < n*n; i++)
        g->matrix[i] = 0;

    for (i = 0; i < k; i++) {
        shuffle(n, t);
        for (j = 0; j < n; j++)
            gm_multi_edge_add(g, j, t[j]);
    }

    free(t);
    return g;
}
