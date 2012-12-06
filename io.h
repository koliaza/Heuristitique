#ifndef IO_H
#define IO_H

#include "graph.h"
#include <stdio.h>

/* returns NULL if there was a problem */
graph_list* read_graph_from_file(const char* filename);

/* Prints a description of the graph in the DOT language
   Suitable for displaying graphs using neato */
void fprint_graph_matrix_dot(FILE* file, graph_matrix* g);

#endif
