// evaluate_from_file.c
// calculates the stable decomposition of H^i(PConf_n(C))
// makes use of a file containing the character table of S_n

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gmp.h>
#include <pthread.h>
#include "../cycle_types.h"
#include "../count_cycle_types.h"
#include "../multiplicities.h"

// we may not have enough memory to read in the entire file
// thus we read in only CHARACTERS_PER_THREAD lines for each thread to process
#define CHARACTERS_PER_THREAD 100

#define BUF_SIZE 100000

typedef struct {
	int n;
	mpz_t *q;
	int num_partitions;
	int modulus;
	int remainder;
	int count;
	mpz_t **characters;
	mpz_t *counts;
	mpz_t **results;
} thread_data;

void *compute_multiplicities(void *void_data) {
	thread_data data = *((thread_data *) void_data);

	mpz_t sum;
	for (int i = 0; ; i++) {
		int index = data.remainder + i * data.modulus;
		if (index >= data.count)
			break;

		mpz_init_set_ui(sum, 0);
		for (int j = 0; j < data.num_partitions; j++) {
			mpz_addmul(sum, data.counts[j], data.characters[index][j]);
			mpz_clear(data.characters[index][j]);
		}
		free(data.characters[index]);

		data.results[index] = get_multiplicities(data.n, sum, *data.q);
	}
	mpz_clear(sum);

	return NULL;
}

int main(int argc, char **argv) {
	int n = atoi(argv[1]);
	int max_i = atoi(argv[2]);
	char *filename = argv[3];
	int threads = atoi(argv[4]);
	int num_primes = argc - 5;
	mpz_t primes[num_primes];
	for (int j = 0; j < num_primes; j++) {
		mpz_init(primes[j]);
		gmp_sscanf(argv[j + 5], "%Zd", primes[j]);
	}

	// check that n does not exceed MAX_N
	if (n > MAX_N) {
		fprintf(stderr, "Error: n=%d is greater than MAX_N=%d\nRecompile with \"make MAX_N=%d\"\n", n, MAX_N, n);
		exit(-1);
	}

	// we only need the cycle_types of n, and even then only for their partitions for printing
	cycle_types *css = compute_cycle_types(n);
	cycle_types cs = css[n];
	for (int j = 0; j < n; j++)
		free_cycle_types(css[j]);

	// open the input file
	FILE *character_table_fp = fopen(filename, "r");

	for (int prime_index = 0; prime_index < num_primes; prime_index++) {

		// calculate counts of polynomials with each cycle type
		mpz_t *counts = count_cycle_types(n, primes[prime_index], cs, threads);

		// open the output files
		FILE **decomp_fps = malloc((max_i + 1) * sizeof(FILE *));
		for (int i = 0; i <= max_i; i++) {
			char decomp_filename[256];
			gmp_sprintf(decomp_filename, "H^%d(PConf_n(C)).out.%Zd", i, primes[prime_index]);
			decomp_fps[i] = fopen(decomp_filename, "w");
		}

		char full_decomp_filename[256];
		gmp_sprintf(full_decomp_filename, "full_decomp.out.%Zd", primes[prime_index]);
		FILE *full_decomp_fp = fopen(full_decomp_filename, "w");

		// read the file chunk by chunk
		int num_chunks = cs.count / (threads * CHARACTERS_PER_THREAD) + 1;
		for (int j = 0; j < num_chunks; j++) {
			int num_lines;
			if (j == num_chunks - 1) {
				num_lines = cs.count % (threads * CHARACTERS_PER_THREAD);
				if (!num_lines)
					break;
			} else {
				num_lines = threads * CHARACTERS_PER_THREAD;
			}

			// reading the file
			mpz_t **characters = malloc(num_lines * sizeof(mpz_t *));
			char **line = malloc(sizeof(char *));
			size_t *line_len = malloc(sizeof(size_t));
			*line_len = BUF_SIZE;
			*line = malloc(*line_len);
			for (int k = 0; k < num_lines; k++) {
				memset(*line, 0, *line_len);

				getline(line, line_len, character_table_fp);

				characters[k] = malloc(cs.count * sizeof(mpz_t));
				int line_index = 0;
				for (int m = 0; m < cs.count; m++) {
					gmp_sscanf(*line + line_index, "%Zd", characters[k][m]);

					// consume string to next character value
					while (line_index < *line_len && (*line)[line_index++] != ' ') {}
				}
			}
			free(*line);
			free(line);
			free(line_len);

			// allocate thread data
			mpz_t **multiplicities = malloc(num_lines * sizeof(mpz_t *));
			pthread_t thread_ids[threads];
			thread_data *data = malloc(threads * sizeof(thread_data));
			for (int k = 0; k < threads; k++) {
				data[k].n = n;
				data[k].q = &primes[prime_index];
				data[k].num_partitions = cs.count;
				data[k].modulus = threads;
				data[k].remainder = k;
				data[k].count = num_lines;
				data[k].characters = characters;
				data[k].counts = counts;
				data[k].results = multiplicities;

				// spawn thread
				pthread_create(&thread_ids[k], NULL, compute_multiplicities, &data[k]);
			}

			// join threads
			for (int k = 0; k < threads; k++)
				pthread_join(thread_ids[k], NULL);

			// characters is no longer needed
			free(characters);

			// print the decompositions for each i up to max_i
			for (int i = 0; i <= max_i; i++) {
				for (int k = 0; k < num_lines; k++) {
					int partition_index = k + j * threads * CHARACTERS_PER_THREAD;
					print_character_multiplicity(decomp_fps[i], cs.partitions[partition_index], multiplicities[k][i]);
				}
			}

			// print the full decomposition
			for (int k = 0; k < num_lines; k++) {
				for (int m = 0; m <= n; m++) {
					gmp_fprintf(full_decomp_fp, "%Zd ", multiplicities[k][m]);
					mpz_clear(multiplicities[k][m]);
				}
				free(multiplicities[k]);
				fprintf(full_decomp_fp, "\n");
			}
			free(multiplicities);
		}

		for (int i = 0; i <= max_i; i++)
			fclose(decomp_fps[i]);
		free(decomp_fps);

		fclose(full_decomp_fp);

		// free counts
		for (int j = 0; j < cs.count; j++)
			mpz_clear(counts[j]);
		free(counts);
	}

	exit(0);
}
