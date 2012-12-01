#ifndef IO_H
#define IO_H

#include "graph.h"

/* returns NULL if there was a problem */
graph_list* read_graph_from_file(const char* filename);

#endif
