// cycle_types.h
// defines functions for computing partitions and cycle types
// NOTE: MAX_N must be defined at compile time

#ifndef CYCLE_TYPES_H
#define CYCLE_TYPES_H

typedef unsigned char byte;

typedef struct {
	byte vals[MAX_N + 1];
} cycle_type, partition;

typedef struct {
	int count;
	int *start_positions;		// start of the set of cycle types with a given max_index
	cycle_type *cycle_types;
	partition *partitions;		// certain functions are easier to compute for partitions
} cycle_types;

typedef union _tree {
	union _tree *node;
	int leaf;
} tree;

void free_cycle_types(cycle_types cs);

cycle_types *compute_cycle_types(int n);

int compare_cycle_types(const void *a_void, const void *b_void);

tree get_partition_index_tree(int n, cycle_types cs);

int get_index(partition p, tree partition_index_tree);

#endif
