#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "random_graph.h"
#include "io.h"

int main(int argc, char **argv) {
    const int n = 42;
    const int k = 12;

    FILE *f;

    graph_matrix *g1m; graph_list *g1l;
    graph_matrix *g2m; graph_list *g2l;
    graph_matrix *g3m; graph_list *g3l;

    srand(time(NULL));
    
    g1m = erdos_renyi_gnm(n, 100);
    g2m = erdos_renyi_gnm(n, 100);
    g3m = random_isomorphic(g1m);

    g1l = graph_matrix_to_list(g1m);
    g2l = graph_matrix_to_list(g2m);
    g3l = graph_matrix_to_list(g3m);

    f = fopen("graph1", "w");
    fprint_graph_list(f, g1l);
    fclose(f);

    f = fopen("graph2", "w");
    fprint_graph_list(f, g2l);
    fclose(f);

    f = fopen("graph3", "w");
    fprint_graph_list(f, g3l);
    fclose(f);

    return 0;
}
