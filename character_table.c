// character_table.c
// allows the character table to be computed on a loosely connected cluster
// prints every modulus entry of the character table of S_n to S_n.modulus.remainder, starting with remainder
// to print the whole table, set modulus=1 remainder=0
// not intended to be well-spaced for human reading

#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include <pthread.h>
#include "characters.h"
#include "cycle_types.h"

typedef struct {
	int n;
	int modulus;
	int remainder;
	cycle_types *css;
	mpz_t **results;
} thread_data;

void *compute_characters(void *void_data) {
	thread_data data = *((thread_data *) void_data);

	cycle_types cs = data.css[data.n];

	for (int i = data.remainder; i < cs.count; i += data.modulus) {
		data.results[i] = character(data.n, i, data.css);
	}

	return NULL;
}

int main(int argc, char **argv) {
	int n = atoi(argv[1]);
	int modulus = atoi(argv[2]);
	int remainder = atoi(argv[3]);
	int threads = atoi(argv[4]);

	cycle_types *css = compute_cycle_types(n);
	cycle_types cs = css[n];

	// number of characters computed by this instance
	int count;
	if (remainder < cs.count % modulus)
		count = cs.count / modulus + 1;
	else
		count = cs.count / modulus;

	mpz_t **results = malloc(count * sizeof(mpz_t *));

	// create thread data
	pthread_t thread_ids[threads];
	thread_data data[threads];
	for (int i = 0; i < threads; i++) {
		data[i].n = n;
		data[i].modulus = modulus * threads;
		data[i].remainder = modulus * i + remainder;
		data[i].css = css;
		data[i].results = results;
	}

	// spawn threads to compute the character table
	for (int i = 0; i < threads; i++)
		pthread_create(&thread_ids[i], NULL, compute_characters, &data[i]);

	// join them
	for (int i = 0; i < threads; i++)
		pthread_join(thread_ids[i], NULL);

	// print the results to a file
	char filename[32];
	sprintf(filename, "S_%d.%d.%d", n, modulus, remainder);
	FILE *fp = fopen(filename, "w");
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < cs.count; j++)
			gmp_fprintf(fp, "%Zd ", results[i][j]);
		fprintf(fp, "\n");
	}
	fclose(fp);

	exit(0);
}
