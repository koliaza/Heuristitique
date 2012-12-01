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
