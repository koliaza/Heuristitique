#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

graph_matrix* gm_alloc(int n) {
    graph_matrix *g;
    
    g = malloc(sizeof(graph_matrix));
    g->n = n;
    g->matrix = malloc(n*n*sizeof(int));
    if (g->matrix == NULL) {
        fprintf(stderr, "Allocation of %d*%d matrix failed\n", n, n);
        exit(1);
    }

    return g;
}

graph_matrix* gm_init_zero(graph_matrix* g) {
    int i;
    for (i = 0; i < g->n * g->n; i++)
        g->matrix[i] = 0;
    return g;
}

void gm_free(graph_matrix* g) {
    free(g->matrix);
    free(g);
}

/* inline all following functions to make faster ? */

int gm_edge(graph_matrix* g, int i, int j) {
    return g->matrix[i*(g->n) + j];
}
void gm_edge_add(graph_matrix* g, int i, int j) {
    g->matrix[i*(g->n) + j] = 1;
    g->matrix[j*(g->n) + i] = 1;
}
void gm_edge_remove(graph_matrix* g, int i, int j) {
    g->matrix[i*(g->n) + j] = 0;
    g->matrix[j*(g->n) + i] = 0;
}
void gm_multi_edge_add(graph_matrix* g, int i, int j) {
    g->matrix[i*(g->n) + j] += 1;
    g->matrix[j*(g->n) + i] += 1;
}
void gm_multi_edge_remove(graph_matrix* g, int i, int j) {
    g->matrix[i*(g->n) + j] -= 1;
    g->matrix[j*(g->n) + i] -= 1;
}


int gm_vertex_degree(graph_matrix* g, int i) {
    int j, d = 0;
    /* maybe factorize this code later into a sum_row function for matrices */
    assert(i < g->n);
    for (j = 0; j < g->n; j++) {
        d += g->matrix[i*g->n + j];
    }
    return d;
}



graph_list* gl_alloc(int n) {
    int i;
    graph_list* g = malloc(sizeof(graph_list));
    g->n = n;
    g->list_array = malloc(n*sizeof(int_list));
    for (i = 0; i < n; i++)
        g->list_array[i] = NULL;
    return g;
}

void gl_free(graph_list* g) {
    int i;
    for (i = 0; i < g->n; i++)
        il_free(g->list_array[i]);
    free(g);
}

int gl_edge(graph_list* g, int i, int j) {
    return il_s_member(i, g->list_array[j]);
}
void gl_edge_add(graph_list* g, int i, int j) {
    g->list_array[i] = il_s_insert_once(j, g->list_array[i]);
    g->list_array[j] = il_s_insert_once(i, g->list_array[j]);
}
void gl_edge_remove(graph_list* g, int i, int j) {
    g->list_array[i] = il_s_remove_once(j, g->list_array[i]);
    g->list_array[j] = il_s_remove_once(i, g->list_array[j]);
}
void gl_multi_edge_add(graph_list* g, int i, int j) {
    g->list_array[i] = il_s_insert(j, g->list_array[i]);
    g->list_array[j] = il_s_insert(i, g->list_array[j]);
}
void gl_multi_edge_remove(graph_list* g, int i, int j) {
    g->list_array[i] = il_s_remove_once(j, g->list_array[i]);
    g->list_array[j] = il_s_remove_once(i, g->list_array[j]);
}
