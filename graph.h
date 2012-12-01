#ifndef GRAPH_H
#define GRAPH_H

#include "util.h"

/* These data structures can represent both simple graphs and multigraphs */

struct graph_matrix {
    int n;
    int *matrix; /* Convention : the (i,j) coefficient of the matrix
                    is matrix[i*n + j] */
};
typedef struct graph_matrix graph_matrix;

graph_matrix* gm_alloc(int n);
void gm_free(graph_matrix* g);
int gm_edge(graph_matrix* g, int i, int j);
void gm_edge_add(graph_matrix* g, int i, int j);
void gm_edge_remove(graph_matrix* g, int i, int j);
void gm_multi_edge_add(graph_matrix* g, int i, int j);
void gm_multi_edge_remove(graph_matrix* g, int i, int j);

struct graph_list {
    int n;
    int_list* list_array;
};
typedef struct graph_list graph_list;

#endif
