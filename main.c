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

    if (!find_isomorphism(g1, g2, NULL))
        printf("=D 1\n");

    int result[10];
    int i;
    for (i = 0; i < 10; i++)
        result[i] = -42;
    if(find_isomorphism(g1, g3, result))
        printf("=D 2\n");
    for (i = 0; i < 10; i++)
        printf("%d ", result[i]);
    putchar('\n');
    
    return 0;
}
