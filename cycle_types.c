// cycle_types_and_partitions.c
// see header file for details

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cycle_types.h"

#define BUF_MIN 4096
#define BUF_RESIZE_FACTOR 1.5

void free_cycle_types (cycle_types cs) {
	free(cs.start_positions);
	free(cs.cycle_types);
	free(cs.partitions);
}

// returns an array contianing cycle types for all t up to n
cycle_types *compute_cycle_types(int n) {
	cycle_types *results = malloc((n + 1) * sizeof(cycle_types));

	// compute cycle_types of 0
	results[0].count = 1;
	results[0].start_positions = calloc(1, sizeof(int));
	results[0].cycle_types = calloc(1, sizeof(cycle_type));
	results[0].partitions = calloc(1, sizeof(partition));

	// compute remaining cycle_types and partitions using previous
	for (int t = 1; t <= n; t++) {
		cycle_types *next = results + t;
		next->count = 0;
		next->start_positions = malloc(t * sizeof(int));

		// allocate the array of cycle types and partitions
		int buf_size = BUF_MIN;
		next->cycle_types = malloc(buf_size * sizeof(cycle_type));
		next->partitions = malloc(buf_size * sizeof(cycle_type));

		// handle the case max_index = 0
		next->cycle_types[0].vals[0] = t;
		for (int i = 1; i < MAX_N; i++) {
			next->cycle_types[0].vals[i] = 0;
		}
		for (int i = 0; i < t; i++) {
			next->partitions[0].vals[i] = 1;
		}
		for (int i = t; i < MAX_N; i++) {
			next->partitions[0].vals[i] = 0;
		}
		next->count = 1;
		next->start_positions[0] = 0;

		// loop over the max_index
		for (int max_index = 1; max_index < t; max_index++) {
			next->start_positions[max_index] = next->count;

			// a cycle_type with sum n and a given max_index is always formed by
			// a cycle_type with sum n - k * (max_index + 1) and smaller maximum index,
			// with vals[max_index] then set to k
			int max_val = max_index + 1;
			int max_k = t / max_val;
			for (int k = 1; k <= max_k; k++) {

				int smaller_sum = t - k * max_val;
				cycle_types old_result = results[smaller_sum];

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
						memcpy((void *) new_cycle_type, (void *) (old_result.cycle_types + i), MAX_N);
						new_cycle_type->vals[max_index] = k;

						// convert new_cycle_type to a partition
						partition *new_partition = next->partitions + next->count;
						memset(&(new_partition->vals), 0, MAX_N);
						int new_partition_index = 0;
						for (int i = MAX_N - 1; i >= 0; i--) {
							for (int j = 0; j < new_cycle_type->vals[i]; j++) {
								new_partition->vals[new_partition_index++] = i + 1;
							}
						}

						// increment count and reallocate if necessary
						if (++(next->count) > buf_size) {
							buf_size = (int) (buf_size * BUF_RESIZE_FACTOR);
							next->cycle_types = realloc(next->cycle_types, buf_size * sizeof(cycle_type));
							next->partitions = realloc(next->partitions, buf_size * sizeof(cycle_type));
						}
					}
				}
			}
		}

		// avoid taking up more space than necessary
		next->cycle_types = realloc(next->cycle_types, next->count * sizeof(cycle_types));
	}

	return results;
}

// agrees with the ordering produced by compute_cycle_types
// BUT only within cycle_types of the same sum
int compare_cycle_types(const void *a_void, const void *b_void) {
	const cycle_type a = *((cycle_type *) a_void);
	const cycle_type b = *((cycle_type *) b_void);
	
	for (int i = MAX_N - 1; i >= 0; i--) {
		if (a.vals[i] < b.vals[i]) {
			return -1;
		} else if (a.vals[i] > b.vals[i]) {
			return 1;
		}
	}

	return 0;
}

int compare_partitions(const void *a_void, const void *b_void) {
	const partition a = *((partition *) a_void);
	const partition b = *((partition *) b_void);

	for (int i = 0; i < MAX_N; i++) {
		if (a.vals[i] < b.vals[i]) {
			return -1;
		} else if (a.vals[i] > b.vals[i]) {
			return 1;
		}
	}

	return 0;
}

int get_index(partition p, cycle_types cs) {
	partition *result_pointer = bsearch(&p, cs.partitions, cs.count, sizeof(partition), compare_partitions);

	return ((int) (result_pointer - cs.partitions));
}

// test functions
#ifdef TEST_CYCLE_TYPES

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
		printf("<-> ");
		partition p = c.partitions[i];
		for (int j = 0; j < MAX_N && p.vals[j]; j++) {
			printf("%d ", p.vals[j]);
		}
		printf("\n");
	}
}

int main(int argc, char **argv) {
	int n;
	if (argc > 1)
		n = atoi(argv[1]);
	else
		n = MAX_N;

	cycle_types *results = compute_cycle_types(n);

	for (int i = 0; i <= n; i++) {
		print_cycle_types(i, results[i]);
	}
}

#endif

