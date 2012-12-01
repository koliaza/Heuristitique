#include "graph.h"

#include <stdio.h>

graph_matrix* alloc_graph_matrix(int n) {
    graph_matrix *g;
    
    g = malloc(sizeof(graph_matrix));
    g->n = n;
    g->matrix = malloc(n*n*sizeof(int));
    if (g->matrix == NULL) {
        fprintf(stderr, "Allocation of %d*%d matrix failed\n", n, n);
        exit(1);
    }

    return p;
}

void free_graph_matrix(graph_matrix* g) {
    free(g->matrix);
    free(g);
}
