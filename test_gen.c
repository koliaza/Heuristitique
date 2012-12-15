#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int generate_iso (int n, int kreg, double p, int m, int mode, int numgraphs, char* prefix){
    FILE *f;
    graph_matrix * gsource;
    graph_list * gcopy;
    int i;
    char * filename = malloc(20*sizeof(char)); 
    
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
    
    for (i = 0; i < numgraphs; i++){
            gcopy = graph_matrix_to_list(random_isomorphic(gsource));
            sprintf(filename,"%s%04d",prefix ,i );
            f = fopen(filename, "w");
            fprint_graph_list(f, gcopy);
            fclose(f);
            free(gcopy);
         }
    free(gsource);
    return 0;
}

int main(int argc, char **argv) {
    int n, numgraphs, mode, iso,i;
    double p = 0.0;
    int kreg = 0, m = 0;
    char* prefix;
    

    char * filename = malloc(20*sizeof(char));
    if (argc != 7) {
        printf("'test_gen n numgraphs (p || m || kreg) mode iso fname' with :\n");
        printf("n : number of vertices, numgraphs : number graphs to generate\n");
        printf("p and m : as in G(n,p) and G(n,m), kreg : degree when k-regular\n");
        printf("mode : 1 for G(n,p), 2 for G(n,m), 3 for G(n,m) multigraph\n");
        printf("     : 4 for k-regular, 5 for k-regular multigraphs\n");
        printf("iso  : 1 for generating graphs isomorphic to the first generated\n");
        printf("prefix  : name prefix for generated graphs\n");
        return 1;
    } /* redundant test but it is better this way*/
    n = atoi(argv[1]);
    numgraphs = atoi(argv[2]);
    p = atof(argv[3]);
    m = atoi(argv[3]);
    kreg = atoi(argv[3]);
    mode = atoi(argv[4]);
    iso = atoi(argv[5]);
    prefix = argv[6];
    srand(time(NULL));
    
  
    if (!iso){ 
        for (i = 1; i < numgraphs; i++){
            sprintf(filename,"%s%04d",prefix, i );
            generate_graph (n, kreg, p, m, mode, filename );
         }
    }
   else {
       generate_iso(n, kreg, p, m, mode, numgraphs, prefix);
   }
   
    return 0;
}
