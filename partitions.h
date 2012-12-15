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
    int *array;
} partition;

partition* init_partition(int n, partition *p);
void free_partition(partition *p);
void refine_partition(partition *part, const int64_t *keys);

typedef struct {
    int_list *boundaries;
    int *array_a;
    int *array_b;
} matched_partitions;

matched_partitions* init_matched_partitions(int n, matched_partitions *pp);
void free_matched_partitions(matched_partitions *pp);
double possible_matchings_count(const int_list *boundaries);

/* return 0 if the partitions become incompatible,
   else 1 if there's no problem */
int refine_matched_partitions(matched_partitions *pp,
                              const int64_t *keys_a, const int64_t *keys_b);

/* return value : number of subsets in partition */
int tag_array_with_partition(partition *part, int *tagged_array);
int tag_arrays_with_matched_partition(matched_partitions *pp,
                                      int *a_array, int *b_array);

#endif
