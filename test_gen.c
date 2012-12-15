#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "random_graph.h"
#include "io.h"
int generate_graph (int n, int kreg, double p, int m, int mode, char* file){
    FILE *f;
    graph_list * g;
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
            g = graph_matrix_to_list(random_k_regular(n,kreg));
            break;
        case 5:
           g = graph_matrix_to_list(random_k_regular_multi(n,kreg));
            break;
    }
    fprint_graph_list(f, g);
    fclose(f);
    free(g);
    return 0;
}

int generate_iso (int n, int kreg, double p, int m, int mode, int numgraphs){
    FILE *f;
    graph_matrix * gsource;
    graph_list * gcopy;
    int i;
    char * filename = malloc(20*sizeof(char)); 
    
    f = fopen("graph_iso_source","w");
    switch (mode){
        case 1:
            gsource = erdos_renyi_gnp(n,p);
            break;
        case 2:
            gsource = erdos_renyi_gnm(n,m);
            break;
        case 3:
            gsource = erdos_renyi_gnm_multi(n,m);
            break;
        case 4:
            gsource = random_k_regular(n,kreg);
            break;
        case 5:
           gsource = random_k_regular_multi(n,kreg);
            break;
    }
    fprint_graph_list(f, graph_matrix_to_list(gsource));
    fclose(f);
     for (i = 0; i < numgraphs; i++){
            gcopy = random_isomorphic(gsource);
            sprintf(filename,"graph%04d",i );
            f = fopen(filename, "w");
            fprint_graph_list(f, gcopy);
            fclose(f);
            free(gcopy);
         }
    free(gsource);
    return 0;
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
    } /* redundant test but it is better this way*/
    n = atoi(argv[1]);
    numgraphs = atoi(argv[2]);
    p = atoi(argv[3]);
    m = atoi(argv[4]);
    kreg = atoi(argv[5]);
    mode = atoi(argv[6]);
    iso = atoi(argv[7]);
    srand(time(NULL));
    
  
    if (!iso){ 
        for (i = 1; i < numgraphs; i++){
            sprintf(filename,"graph%04d",i );
            generate_graph (n, kreg, p, m, mode, filename);
         }
   else {
       generate_iso(n, kreg, p, m, mode, numgraphs);
   }
   
    return 0;
}
