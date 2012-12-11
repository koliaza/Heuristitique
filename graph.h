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

/* gm = graph matrix */
graph_matrix* gm_alloc(int n);
graph_matrix* gm_init_zero(graph_matrix* g);
void gm_free(graph_matrix* g);
int gm_edge(const graph_matrix* g, int i, int j);

void gm_edge_add(graph_matrix* g, int i, int j);
void gm_edge_remove(graph_matrix* g, int i, int j);
void gm_multi_edge_add(graph_matrix* g, int i, int j);
void gm_multi_edge_remove(graph_matrix* g, int i, int j);

int gm_vertex_degree(const graph_matrix* g, int i);

typedef struct graph_list graph_list;
struct graph_list {
    int n;
    int_list** list_array;
};

/* gl = graph list */
graph_list* gl_alloc(int n);
void gl_free(graph_list* g);
int gl_edge(const graph_list* g, int i, int j);

void gl_edge_add(graph_list* g, int i, int j);
void gl_edge_remove(graph_list* g, int i, int j);
void gl_multi_edge_add(graph_list* g, int i, int j);
void gl_multi_edge_remove(graph_list* g, int i, int j);

int gl_vertex_degree(const graph_list* g, int i);


graph_list* graph_matrix_to_list(const graph_matrix *gm);
graph_matrix* graph_list_to_matrix(const graph_list *gl);

#endif
