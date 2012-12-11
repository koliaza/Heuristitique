#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "random_graph.h"
#include "io.h"

const int n = 12;

int main(int argc, char **argv) {
    int c;
    FILE *f;
    graph_matrix *gm1 = NULL, *gm2 = NULL;
    graph_list *gl1 = NULL, *gl2 = NULL;
    
    srand(time(NULL));
    gm1 = erdos_renyi_gnp(n, 0.42);
    gl1 = graph_matrix_to_list(gm1);

    f = fopen("foobar", "w");
    if (f == NULL) exit(1);
    fprint_graph_list(f, gl1);
    fclose(f);

    gl2 = read_graph_from_file("foobar", NULL);
    gm2 = graph_list_to_matrix(gl2);

    if (memcmp(gm1->matrix, gm2->matrix, n*n*sizeof(int)) == 0) {
        printf("Success!\n");
    } else {
        printf("Failure :-(\n");
        print_matrix(n, gm1->matrix);
        putchar('\n');
        print_matrix(n, gm2->matrix);
    }

    gm_free(gm1);
    gm_free(gm2);
    gl_free(gl1);
    gl_free(gl2);

    return 0;
}
