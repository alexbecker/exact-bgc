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

typedef struct {
	volatile int max_n_computed;
	cycle_types results[MAX_N + 1];
} workspace;

workspace *alloc_workspace();

void *compute_cycle_types(workspace *w);

int compare_cycle_types(const void *a_void, const void *b_void);

int get_index(partition p, cycle_types cs);

#endif
