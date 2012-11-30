#ifndef GRAPH_H
#define GRAPH_H

/* These data structures can represent multigraphs */

struct graph_matrix {
    int n;
    int *matrix;
};
typedef struct graph_matrix graph_matrix;

struct int_list {
    int x;
    int_list *next;
};
typedef struct int_list int_list;

struct graph_list {
    int n;
    int_list* adj_list;
};
typedef struct graph_list graph_list;

#endif
