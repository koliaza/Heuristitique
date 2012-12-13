#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "random_graph.h"
#include "io.h"
#include "pn_heuristic.h"

int main(int argc, char **argv) {
    const int n = 42;
    
    graph_matrix *g1 = erdos_renyi_gnp(n, 0.666 - 0.12);
    graph_matrix *g2 = erdos_renyi_gnp(n, 0.666 - 0.12);
    graph_matrix *g3 = random_isomorphic(g1);

    if (!find_isomorphism(g1, g2, NULL) && find_isomorphism(g1, g3, NULL))
        printf("=D\n");
    else
        printf("T_T\n");
    
    return 0;
}
