#ifndef RANDOM_GRAPH_H
#define RANDOM_GRAPH_H

#include "graph.h"

/* Note : the G(n,p) model generates simple graphs
   whereas the G(n,m) model generates multigraphs */
graph_matrix* erdos_renyi_gnp(int n, double p);
graph_matrix* erdos_renyi_gnm(int n, int m);

/* Warning : random_k_regular runs in non-deterministic time ! */
graph_matrix* random_k_regular(int n, int k);
graph_matrix* random_k_regular_multi(int n, int k);

#endif
