#ifndef GRAPH_H
#define GRAPH_H

#include "util.h"

/* These data structures can represent both simple graphs and multigraphs */

typedef struct graph_matrix graph_matrix;
struct graph_matrix {
    int n;
    int *matrix; /* Convention : the (i,j) coefficient of the matrix
                    is matrix[i*n + j] */
};

graph_matrix* gm_alloc(int n);
graph_matrix* gm_init_zero(graph_matrix* g);
void gm_free(graph_matrix* g);
int gm_edge(graph_matrix* g, int i, int j);
void gm_edge_add(graph_matrix* g, int i, int j);
void gm_edge_remove(graph_matrix* g, int i, int j);
void gm_multi_edge_add(graph_matrix* g, int i, int j);
void gm_multi_edge_remove(graph_matrix* g, int i, int j);

typedef struct graph_list graph_list;
struct graph_list {
    int n;
    int_list** list_array;
};

#endif
