#ifndef IO_H
#define IO_H

#include "graph.h"
#include <stdio.h>

/* for debugging purposes */
void print_matrix(int n, int *a);
void print_list(const int_list *l);


/* TODO : fix inconsistency in interface
   file handle vs. filename */

/* Functions for reading and writing graphs from/to files
   in the format specified in the subject */
graph_list* fget_graph_list(FILE *f, int* counter);
void fprint_graph_list(FILE* f, const graph_list* g);


/* Prints a description of the graph in the DOT language
   Suitable for displaying graphs using neato */
void fprint_graph_matrix_dot(FILE* file, const graph_matrix* g);

#endif
