#ifndef PARTITIONS_H
#define PARTITIONS_H

/*** Partitions / Equivalence classes on the vertices of a graph ***/

/* Partition = array of elements + list of boundaries
   format chosen for its suitability in incremental updates
 */

/* add a single partition struct later ? might be of use... */

#include <stdint.h>
#include "util.h"

typedef struct {
    int_list *boundaries;
    int *array_a;
    int *array_b;
} matched_partitions;

matched_partitions* init_matched_partitions(int n, matched_partitions *pp);
void free_matched_partitions(matched_partitions *pp);
double possible_matchings_count(const int_list *boundaries);
int refine_matched_partitions(matched_partitions *pp,
                              const int64_t *keys_a, const int64_t *keys_b);


#endif
