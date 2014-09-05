// cycle_types_and_partitions.h
// defines functions for computing partitions and cycle types
// NOTE: MAX_N must be defined at compile time

#ifndef CTAP_H
#define CTAP_H

typedef unsigned char byte;

typedef struct {
	byte vals[MAX_N + 1];
} cycle_type;

typedef struct {
	int count;
	int *start_positions;		// start of the set of cycle types with a given max_index
	cycle_type *cycle_types;
} cycle_types;

typedef struct {
	volatile int max_n_computed;
	cycle_types results[MAX_N + 1];
} workspace;

workspace *alloc_workspace();

void compute_cycle_types(workspace *w);

#endif
