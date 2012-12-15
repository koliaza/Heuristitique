#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "random_graph.h"
#include "io.h"
int generate_graph (int n, int kreg, double p, int m, int mode, char* file){
    FILE *f;
    graph_matrix * g;
    f = fopen(file, "w");

    switch (mode){
        case 1:
            g = graph_matrix_to_list(erdos_renyi_gnp(n,p));
            break;
        case 2:
            g = graph_matrix_to_list(erdos_renyi_gnm(n,m));
            break;
        case 3:
            g = graph_matrix_to_list(erdos_renyi_gnm_multi(n,m));
            break;
        case 4:
            g = graph_matrix_to_list(random_k_regular(n,k));
            break;
        case 5:
           g = graph_matrix_to_list(random_k_regular_multi(n,k));
            break;
    }
    fprint_graph_list(f, g);
    fclose(f);
    free(g);
}

int generate_iso (int n, int kreg, double p, int m, int mode){
    FILE *f;
    graph_matrix * gsource;
    graph_matrix * gcopy;
    char * filename = malloc(20*sizeof(char)); 
    
    f = fopen("graph_iso_source","w");
    switch (mode){
        case 1:
            g = graph_matrix_to_list(erdos_renyi_gnp(n,p));
            break;
        case 2:
            g = graph_matrix_to_list(erdos_renyi_gnm(n,m));
            break;
        case 3:
            g = graph_matrix_to_list(erdos_renyi_gnm_multi(n,m));
            break;
        case 4:
            g = graph_matrix_to_list(random_k_regular(n,k));
            break;
        case 5:
           g = graph_matrix_to_list(random_k_regular_multi(n,k));
            break;
    }
    fprint_graph_list(f, gsource);
    fclose(f);
     for (i = 0; i < numgraphs; i++){
            gcopy = random_isomorphic(g);
            sprintf(filename,"graph%04d",i );
            f = fopen(filename, "w");
            fprint_graph_list(f, gcopy);
            fclose(f);
            free(gcopy);
         }
    free(gsource);
}

int main(int argc, char **argv) {
    int n, kreg, numgraphs, m, mode, iso,i;
    double p = 0.0;

    char * filename = malloc(20*sizeof(char));
    if ((strcmp(argv[1], "-help") == 0) || (argc != 8)){
        printf("'test_gen n numgraphs p m kreg mode iso' with : /n");
        printf("n : number of vertices, numgraphs : number graphs to generate /n");
        printf("p and m : as in G(n,p) and G(n,m), kreg : degree when kregular  /n");
        printf("mode : 1 for G(n,p), 2 for G(n,m), 3 for G(n,m) multigraph/n");
        printf("     : 4 for k-regular, 5 for k-regular multigraphs ");
        printf("iso  : 1 for generating graphs isomorphic to the first generated");
        return 0;
    }
    n = argv[1];
    numgraphs = argv[2];
    p = argv[3];
    m = argv[4];
    kreg = argv[5];
    mode = argv[6];
    iso = argv[7];
    srand(time(NULL));
    
  
    if !(iso){ 
        for (i = 0; i < numgraphs; i++){
            sprintf(filename,"graph%04d",i );
            generate_graph (n, kreg, p, m, mode, filename);
         }
   else generate_iso(n, kreg, p, m, mode);
   
    return 0;
}
