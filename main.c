#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "io.h"
#include "pn_heuristic.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: heuristitique graph1_file graph2_file\n");
    }

    graph_list *g1 = NULL;
    graph_list *g2 = NULL;

    FILE *f;

    f = fopen(argv[1], "r");
    if (f == NULL) {
        fprintf(stderr, "%s : File not found\n", argv[1]);
        exit(1);
    }
    g1 = fget_graph_list(f, NULL);
    fclose(f);

    f = fopen(argv[2], "r");
    if (f == NULL) {
        fprintf(stderr, "%s : File not found\n", argv[2]);
        exit(1);
    }
    g2 = fget_graph_list(f, NULL);
    fclose(f);

    int *isom = malloc(g1->n * sizeof(int));
    int i;

    if (find_isomorphism(g1, g2, isom)) {
        printf("Here is an isomorphism between the 2 graphs:\n");
        for (i = 0; i < g1->n; i++) {
            printf("%d -> %d\n", i, isom[i]);
        }
    } else {
        printf("The 2 graphs are not isomorphic.\n");
    }
    
    if (g1 != NULL) gl_free(g1);
    if (g2 != NULL) gl_free(g2);
    free(isom);
    return 0;
}
