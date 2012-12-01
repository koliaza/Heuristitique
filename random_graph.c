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
            if (coin_flip(p)) {
                g->matrix[i*n+j] = 1;
                g->matrix[j*n+i] = 1;
            } else {
                g->matrix[i*n+j] = 0;
                g->matrix[j*n+i] = 0;
            }
        }
    }
    
    return g;
}
