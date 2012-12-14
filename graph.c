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

int gm_edge(const graph_matrix* g, int i, int j) {
    return g->matrix[i*(g->n) + j];
}
void gm_edge_add(graph_matrix* g, int i, int j) {
    g->matrix[i*(g->n) + j] = 1;
    if (i != j) g->matrix[j*(g->n) + i] = 1;
}
void gm_edge_remove(graph_matrix* g, int i, int j) {
    g->matrix[i*(g->n) + j] = 0;
    if (i != j) g->matrix[j*(g->n) + i] = 0;
}
void gm_multi_edge_add(graph_matrix* g, int i, int j) {
    g->matrix[i*(g->n) + j] += 1;
    if (i != j) g->matrix[j*(g->n) + i] += 1;
}
void gm_multi_edge_remove_once(graph_matrix* g, int i, int j) {
    if (gm_edge(g,i,j)) {
        g->matrix[i*(g->n) + j] -= 1;
        if (i != j) g->matrix[j*(g->n) + i] -= 1;
    }
}



int gm_vertex_degree(const graph_matrix* g, int i) {
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

int gl_edge(const graph_list* g, int i, int j) {
    return il_s_member(i, g->list_array[j]);
}
void gl_edge_add(graph_list* g, int i, int j) {
    g->list_array[i] = il_s_insert_once(j, g->list_array[i]);
    if (i != j) g->list_array[j] = il_s_insert_once(i, g->list_array[j]);
}
void gl_edge_remove(graph_list* g, int i, int j) {
    g->list_array[i] = il_s_remove_once(j, g->list_array[i]);
    if (i != j) g->list_array[j] = il_s_remove_once(i, g->list_array[j]);
}
void gl_multi_edge_add(graph_list* g, int i, int j) {
    g->list_array[i] = il_s_insert(j, g->list_array[i]);
    if (i != j) g->list_array[j] = il_s_insert(i, g->list_array[j]);
}
void gl_multi_edge_remove_once(graph_list* g, int i, int j) {
    g->list_array[i] = il_s_remove_once(j, g->list_array[i]);
    if (i != j) g->list_array[j] = il_s_remove_once(i, g->list_array[j]);
}
void gl_multi_edge_remove_all(graph_list* g, int i, int j) {
    g->list_array[i] = il_s_remove_all(j, g->list_array[i]);
    if (i != j) g->list_array[j] = il_s_remove_all(i, g->list_array[j]);
}
int gl_vertex_degree(const graph_list* g, int i) {
    assert(i < g->n);
    return il_length(g->list_array[i]);
}

void gl_connected_components_aux(const graph_list *g, int *result,
                                 int vertex, int *component_counter) {
    int_list *p;
    result[vertex] = *component_counter;
    for (p = g->list_array[vertex]; p != NULL; p = p->next) {
        if (result[p->x] == -1) {
            gl_connected_components_aux(g, result, p->x, component_counter);
        }
    }
}

int gl_connected_components(const graph_list *g, int *result) {
    int i;
    int component_counter;

    for (i = 0; i < g->n; i++) {
        result[i] = -1;
    }
    
    i = 0;
    component_counter = -1;
    while (i < g->n) {
        if (result[i] != -1) {
            i++;
        } else {
            component_counter++;
            gl_connected_components_aux(g, result, i, &component_counter);
        }
    }

    return component_counter;
}

int gl_equal(const graph_list *g_a, const graph_list *g_b) {
    int i;
    if (g_a->n != g_b->n) return 0;
    for (i = 0; i < g_a->n; i++) {
        if (!il_equal(g_a->list_array[i], g_b->list_array[i]))
            return 0;
    }
    return 1;
}


graph_list* graph_matrix_to_list(const graph_matrix *gm) {
    int i, j, k;
    graph_list *gl = gl_alloc(gm->n);
    for (i = 0; i < gm->n; i++) {
        for (j = i; j < gm->n; j++) {
            for (k = 0; k < gm_edge(gm, i, j); k++)
                gl_multi_edge_add(gl, i, j);
        }
    }
    return gl;
}

graph_matrix* graph_list_to_matrix(const graph_list *gl) {
    int i;
    int_list *p;
    graph_matrix *gm = gm_init_zero(gm_alloc(gl->n));
    for (i = 0; i < gl->n; i++) {
        for (p = gl->list_array[i]; p != NULL; p = p->next) {
            gm_multi_edge_add(gm, i, p->x);
        }
    }
    /* every edge is added twice */
    for (i = 0; i < gm->n * gm->n; i++) {
        gm->matrix[i] /= 2;
    }
    return gm;
}
