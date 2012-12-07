#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "random_graph.h"
#include "io.h"

int main(int argc, char **argv) {
    int i;
    char s[128];
    FILE *f;
    graph_matrix* g;
    
    srand(time(NULL));

/*    for (i = 0; i < 100; i++) {
        sprintf(s, "random-20-regular-%02d.dot", i);
        f = fopen(s, "w");
        if (f == NULL) break;

        g = random_k_regular(50,20);
        fprint_graph_matrix_dot(f, g);
        gm_free(g);
    }
*/

    g = random_k_regular(10,3);
    
    for (i = 0; i < 10; i++) {
        printf("deg(V_%d) = %d\n", i, gm_vertex_degree(g,i));
    }

    gm_free(g);

    return 0;
}
