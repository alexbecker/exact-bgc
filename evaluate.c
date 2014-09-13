// evaluate.c
// calculates the stable decomposition of H^i(PConf_n(C))

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "cycle_types.h"
#include "count_cycle_types.h"
#include "characters.h"
#include "multiplicities.h"

typedef struct {
	int n;
	int modulus;
	int remainder;
	cycle_types *css;
	tree *trees;
	mpz_t *counts;
	mpz_t *sums;
} thread_data;

// compute the sums over all polynomials of the values of the characters
// called by threads spawned in main
void *compute_sums(void *void_data) {
	thread_data data = *((thread_data *) void_data);

	int count = data.css[data.n].count;
	for (int i = data.remainder; i < count; i += data.modulus) {
		mpz_t *ith_character = character(data.n, i, data.css, data.trees);

		mpz_init_set_ui(data.sums[i], 0);
		for (int j = 0; j < count; j++) {
			mpz_addmul(data.sums[i], data.counts[j], ith_character[j]);
			mpz_clear(ith_character[j]);
		}
		free(ith_character);
	}

	return NULL;
}

// passed the values n, max_i, number of threads to use, and a list of primes
// computes the decomposition of H^i(PConf(C^n)) up to MAX_N/4 
int main(int argc, char **argv) {
	int n = atoi(argv[1]);
	int max_i = atoi(argv[2]);
	int threads = atoi(argv[3]);
	int num_primes = argc - 4;
	mpz_t primes[num_primes];
	for (int j = 0; j < num_primes; j++) {
		mpz_init(primes[j]);
		gmp_sscanf(argv[j + 4], "%Zd", primes[j]);
	}

	// check that n does not exceed MAX_N
	if (n > MAX_N) {
		fprintf(stderr, "Error: n=%d is greater than MAX_N=%d\nRecompile with \"make MAX_N=%d\"\n", n, MAX_N, n);
		exit(-1);
	}

	cycle_types *css = compute_cycle_types(n);
	cycle_types cs = css[n];

	tree *trees = malloc((n + 1) * sizeof(tree));
	for (int j = 0; j <= n; j++)
		trees[j] = get_partition_index_tree(j, css[j]);

	for (int prime_index = 0; prime_index < num_primes; prime_index++) {
		mpz_t q;
		mpz_init_set(q, primes[prime_index]);

		// compute counts for each cycle_type
		mpz_t *counts = count_cycle_types(n, q, cs, threads);

		// spawn threads to compute the sums over cycle types
		mpz_t *sums = malloc(cs.count * sizeof(mpz_t));
		pthread_t sums_thread_id[threads];
		thread_data data[threads];
		for (int j = 0; j < threads; j++) {
			data[j].n = n;
			data[j].modulus = threads;
			data[j].remainder = j;
			data[j].counts = counts;
			data[j].css = css;
			data[j].trees = trees;
			data[j].sums = sums;
			pthread_create(&sums_thread_id[j], NULL, compute_sums, &data[j]);
		}

		for (int j = 0; j < threads; j++) {
			pthread_join(sums_thread_id[j], NULL);
		}

		// get multiplicities
		mpz_t **multiplicities = malloc(cs.count * sizeof(mpz_t *));
		for (int j = 0; j < cs.count; j++) {
			multiplicities[j] = get_multiplicities(n, sums[j], q);
			mpz_clear(sums[j]);
		}
		free(sums);

		// free counts
		for (int j = 0; j < cs.count; j++)
			mpz_clear(counts[j]);
		free(counts);

		// print the decomposition for i up to max_i
		for (int i = 0; i <= max_i; i++) {
			char decomp_filename[256];
			gmp_sprintf(decomp_filename, "H^%d(PConf_n(C)).out.%Zd", i, primes[prime_index]);
			FILE *fp = fopen(decomp_filename, "w");
			for (int j = 0; j < cs.count; j++) {
				print_character_multiplicity(fp, cs.partitions[j], multiplicities[j][i]);
			}
			fclose(fp);
		}

		// print the full decomposition
		char full_decomp_filename[256];
		gmp_sprintf(full_decomp_filename, "full_decomp.out.%Zd", primes[prime_index]);
		FILE *fp = fopen(full_decomp_filename, "w");
		for (int j = 0; j < cs.count; j++) {
			for (int k = 0; k <= n; k++) {
				gmp_fprintf(fp, "%Zd ", multiplicities[j][k]);
				mpz_clear(multiplicities[j][k]);
			}
			free(multiplicities[j]);
			fprintf(fp, "\n");
		}
		free(multiplicities);
		fclose(fp);

		mpz_clear(q);
	}

	exit(0);
}
