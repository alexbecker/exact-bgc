// cycle_types_and_partitions.c
// see header file for details

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cycle_types_and_partitions.h"

#define BUF_MIN 4096
#define BUF_RESIZE_FACTOR 1.5

workspace *alloc_workspace() {
	workspace *result = malloc(sizeof(workspace));
	result->max_n_computed = -1;
	
	return result;
}

void compute_cycle_types(workspace *w) {
	// compute cycle_types of 0
	w->results[0].count = 1;
	w->results[0].start_positions = calloc(1, sizeof(int));
	w->results[0].cycle_types = calloc(1, sizeof(cycle_type));
	w->max_n_computed = 0;

	// compute remaining cycle_types and partitions using previous
	for (int n = 1; n <= MAX_N; n++) {
		cycle_types *next = w->results + n;
		next->count = 0;
		next->start_positions = malloc(n * sizeof(int));

		// allocate the array of cycle types
		int buf_size = BUF_MIN;
		next->cycle_types = malloc(buf_size * sizeof(cycle_type));

		// handle the case max_index = 0
		next->cycle_types[0].vals[0] = n;
		for (int i = 1; i < MAX_N; i++) {
			next->cycle_types[0].vals[i] = 0;
		}
		next->count = 1;
		next->start_positions[0] = 0;

		// loop over the max_index
		for (int max_index = 1; max_index < n; max_index++) {
			next->start_positions[max_index] = next->count;

			// a cycle_type with sum n and a given max_index is always formed by
			// a cycle_type with sum n - k * (max_index + 1) and smaller maximum index,
			// with vals[max_index] then set to k
			int max_val = max_index + 1;
			int max_k = n / max_val;
			for (int k = 1; k <= max_k; k++) {

				int smaller_sum = n - k * max_val;
				cycle_types old_result = w->results[smaller_sum];

				// the "|| smaller_max_index == 0" is to deal with the special case where
				// we are building on the zero cycle type
				for (int smaller_max_index = 0; smaller_max_index < max_index && (smaller_max_index < smaller_sum || smaller_max_index == 0); smaller_max_index++) {

					int start_index = old_result.start_positions[smaller_max_index];
					int end_index;
					if (smaller_max_index < smaller_sum - 1) {
						end_index = old_result.start_positions[smaller_max_index + 1];
					} else {
						end_index = old_result.count;
					}

					for (int i = start_index; i < end_index; i++) {

						// old_result.cycle_types[i] has sum n - k * max_val
						// we make it into a cycle type of sum n by setting vals[max_index] = k
						cycle_type *new_cycle_type = next->cycle_types + next->count;
						memcpy((void *) new_cycle_type, (void *) (old_result.cycle_types + i), MAX_N + sizeof(short));
						new_cycle_type->vals[max_index] = k;

						// increment count and reallocate if necessary
						if (++(next->count) > buf_size) {
							buf_size = (int) (buf_size * BUF_RESIZE_FACTOR);
							next->cycle_types = realloc(next->cycle_types, buf_size * sizeof(cycle_type));
						}
					}
				}
			}
		}

		// avoid taking up more space than necessary
		next->cycle_types = realloc(next->cycle_types, next->count * sizeof(cycle_types));

		// declare that we are done computing n
		w->max_n_computed = n;
	}
}

// test functions
#ifdef TEST_CTAP

void print_cycle_types(int n, cycle_types c) {
	printf("start_positions: ");
	for (int i = 0; i < n; i++) {
		printf("%d ", c.start_positions[i]);
	}
	printf("\n");

	for (int i = 0; i < c.count; i++) {
		cycle_type ct = c.cycle_types[i];
		for (int j = 0; j < MAX_N; j++) {
			printf("%d ", ct.vals[j]);
		}
		printf("\n");
	}
}

int main(int argc, char **argv) {
	workspace *w = alloc_workspace();
	compute_cycle_types(w);

	for (int n = 0; n <= MAX_N; n++) {
		print_cycle_types(n, w->results[n]);
	}
}

#endif

