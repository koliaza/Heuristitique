#include "io.h"

#include <stdio.h>
#include "graph.h"

/* works well for small numbers */
void print_matrix(int n, int *a) {
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%3d", a[i*n+j]);
        }
        putchar('\n');
    }
}

void print_list(const int_list *l) {
    putchar('[');
    for (; l != NULL; l = l->next) {
        printf("%d,", l->x);
    } /* who gives a sh** about trailing commas ? */
    putchar(']');
}


/* The current implementation will completely fail if the file provided
   does not follow the spec */
graph_list* fget_graph_list(FILE *f, int* counter) {
    int n, m, i, j, tmp;
    graph_list *g;

    if (counter != NULL) *counter = 0;

    /* scanf is unsafe but who cares ? */
    fscanf(f, "%d", &n);
    g = gl_alloc(n); /* all entries already initialized to NULL */
    
    for (i = 0; i < n; i++) {
        fscanf(f, "%d", &m);
        for (j = 0; j < m; j++) {
            fscanf(f, "%d", &tmp);
            g->list_array[i] = il_s_insert(tmp, g->list_array[i]);
            if (counter != NULL) (*counter)++;
        }
    }

    if (counter != NULL) *counter = (*counter) / 2;
    
    return g;
}

void fprint_graph_list(FILE* f, const graph_list* g) {
    int i;
    int_list *p;
    
    fprintf(f, "%d\n", g->n);
    for (i = 0; i < g->n; i++) {
        fprintf(f, "%d", gl_vertex_degree(g, i));
        for (p = g->list_array[i]; p != NULL; p = p->next) {
            fprintf(f, " %d", p->x);
        }
        fputc('\n', f);
    }
}

/** for graphviz visualization **/
void fprint_graph_matrix_dot(FILE* f, const graph_matrix* g) {
    int i, j;
    
    fputs("graph G {\n\t", f);
    for (i = 0; i < g->n; i++) {
        fprintf(f, "%d; ", i);
    }
    fputc('\n', f);
    for (i = 0; i < g->n; i++) {
        for (j = 0; j < g->n ; j++) {
            if (gm_edge(g, i, j)) {
                fprintf(f, "\t%d -- %d\n", i, j);
            }
        }
    }
    fputc('}',f);                
}

