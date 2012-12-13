#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "io.h"
#include "pn_heuristic.h"

int main(int argc, char *argv[]) {
    graph_list *g1;
    graph_list *g2;
    graph_list *g3;

    FILE *f;

    f = fopen("graph1", "r");
    if (f == NULL) {
        fprintf(stderr, "%s : File not found\n", argv[1]);
        exit(1);
    }
    g1 = fget_graph_list(f, NULL);
    fclose(f);

    f = fopen("graph2", "r");
    if (f == NULL) {
        fprintf(stderr, "%s : File not found\n", argv[1]);
        exit(1);
    }
    g2 = fget_graph_list(f, NULL);
    fclose(f);

    f = fopen("graph3", "r");
    if (f == NULL) {
        fprintf(stderr, "%s : File not found\n", argv[1]);
        exit(1);
    }
    g3 = fget_graph_list(f, NULL);
    fclose(f);

    if (!find_isomorphism(g1, g2, NULL) && find_isomorphism(g1, g3, NULL))
        printf("=D\n");
    else
        printf("T_T\n");
    
    return 0;
}
