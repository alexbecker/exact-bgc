// evaluate.c
// calculates the stable decomposition of H^i(PConf_n(C))

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "cycle_types.h"
#include "count_cycle_types.h"
#include "characters.h"

typedef struct {
	int n;
	int modulus;
	int remainder;
	mpz_t *counts;
	workspace *w;
	mpz_t *sums;
} thread_data;

// get the multiplicities mod q of a character in H^i(PConf(C^n)) for all i
mpz_t *get_multiplicities(int n, mpz_t sum, mpz_t q) {
	mpz_t *results = malloc((n + 1) * sizeof(mpz_t));

	mpz_t q_power, q_power_minus_1, round_factor, result;
	mpz_init_set(q_power, q);
	mpz_init_set_ui(q_power_minus_1, 1);
	mpz_init(round_factor);
	mpz_init(result);
	for (int i = n; i >= 0; i--) {
		mpz_init(result);
		mpz_mod(result, sum, q_power);
		if (i % 2 == 1) {
			mpz_sub(result, q_power, result);
			mpz_mod(result, result, q_power);
		}
		mpz_fdiv_q_ui(round_factor, q_power_minus_1, 2);
		mpz_add(result, result, round_factor);
		mpz_fdiv_q(result, result, q_power_minus_1);

		mpz_init(results[i]);
		mpz_mod(results[i], result, q);

		mpz_mul(q_power, q_power, q);
		mpz_mul(q_power_minus_1, q_power_minus_1, q);
	}
	mpz_clear(q_power);
	mpz_clear(q_power_minus_1);
	mpz_clear(round_factor);
	mpz_clear(result);

	return results;
}

// compute the sums over all polynomials of the values of the characters
// called by threads spawned in main
void *compute_sums(void *void_data) {
	thread_data data = *((thread_data *) void_data);

	int count = data.w->results[data.n].count;
	for (int i = data.remainder; i < count; i += data.modulus) {
		mpz_t *ith_character = character(data.n, i, data.w);

		mpz_init_set_ui(data.sums[i], 0);
		for (int j = 0; j < count; j++) {
			mpz_addmul(data.sums[i], data.counts[j], ith_character[j]);
			mpz_clear(ith_character[j]);
		}
		free(ith_character);
	}

	return NULL;
}

// prints the partition corresponding to the character, and the multiplicity, in readable format
void print_character_multiplicity(FILE *fp, partition p, mpz_t multiplicity) {
	if (!mpz_cmp_ui(multiplicity, 0))
		return;

	fprintf(fp, "V(");
	int i = 1;
	for (; i + 1 < MAX_N && p.vals[i + 1]; i++) {
		fprintf(fp, "%d,", p.vals[i]);
	}
	if (p.vals[i])
		fprintf(fp, "%d): ", p.vals[i]);
	else
		fprintf(fp, "0): ");
	gmp_fprintf(fp, "%Zd\n", multiplicity);
}

void *compute_cycle_types_threadable(void *void_data) {
	workspace *w = void_data;
	compute_cycle_types(w);
	return NULL;
}

// passed the number of threads to use followed by a list of primes
// computes the decomposition of H^i(PConf(C^n)) up to MAX_N/4 
int main(int argc, char **argv) {
	int threads = atoi(argv[1]), num_primes = argc - 2;
	mpz_t primes[num_primes];
	for (int i = 0; i < num_primes; i++) {
		mpz_init(primes[i]);
		gmp_sscanf(argv[i + 2], "%Zd", primes[i]);
	}

	workspace *w = alloc_workspace();

	// start computing the cycle_types in a new thread
	pthread_t workspace_thread_id;
	pthread_create(&workspace_thread_id, NULL, compute_cycle_types_threadable, w);
	pthread_join(workspace_thread_id, NULL);

	for (int n = 0; n <= MAX_N; n += 4) {
		int i = n / 4;

		// wait for w->result[n] to be computed
		while (w->max_n_computed < n) {}

		cycle_types cs = w->results[n];

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
				data[j].w = w;
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
			}

			// print both the decomposition for i
			char decomp_filename[64];
			gmp_sprintf(decomp_filename, "H^%d(PConf_n(C)).out.%Zd", i, primes[prime_index]);
			FILE *fp = fopen(decomp_filename, "w");
			for (int j = 0; j < cs.count; j++) {
				print_character_multiplicity(fp, cs.partitions[j], multiplicities[j][i]);
			}
			fclose(fp);

			// print the full decomposition
			char full_decomp_filename[64];
			gmp_sprintf(full_decomp_filename, "full_decomp_%d.out.%Zd", i, primes[prime_index]);
			fp = fopen(full_decomp_filename, "w");
			for (int j = 0; j < cs.count; j++) {
				for (int k = 0; k <= n; k++) {
					gmp_fprintf(fp, "%Zd ", multiplicities[j][k]);
				}
				fprintf(fp, "\n");
			}
			fclose(fp);

			mpz_clear(q);
		}
	}
}
