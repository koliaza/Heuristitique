#include "io.h"

#include <stdio.h>

graph_list* read_graph_from_file(const char* filename) {
    FILE *f;
    int n;
    graph_list *g;

    f = fopen(filename, "r");
    if (f == NULL) return NULL;

    /* code later */

    fclose(f);
    return g;
}

void fprint_graph_matrix_dot(FILE* f, graph_matrix* g) {
    int i, j;
    
    fputs("graph G {\n\t", f);
    for (i = 0; i < g->n; i++) {
        fprintf(f, "%d; ", i);
    }
    fputc('\n', f);
    for (i = 0; i < g->n; i++) {
        for (j = i+1; j < g->n ; j++) {
            if (gm_edge(g, i, j)) {
                fprintf(f, "\t%d -- %d\n", i, j);
            }
        }
    }
    fputc('}',f);                
}
