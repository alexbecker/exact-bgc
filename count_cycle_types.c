// count_cycle_types.c
// see header file for details

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "count_cycle_types.h"

// data for the threads spawned in num_cycle_types
typedef struct {
	int n;
	int modulus;
	int remainder;
	cycle_types c;
	mpz_t **chooses;
	mpz_t *results;
} thread_data;

int mobius(int n) {
	if (n == 1)
		return 1;

	// trial division -- efficient enough given that this is not the bottleneck
	int sqrt = (int) pow((double) n + 1, 0.5);
	for (int i = 2; i <= sqrt; i++) {
		if (n % i == 0) {
			int new = n / i;
			if (new % i == 0)
				return 0;
			return -1 * mobius(new);
		}
	}

	return -1;
}

mpz_t *num_irreducibles(int n, mpz_t q) {
	mpz_t *result = malloc(sizeof(mpz_t));
	mpz_init(*result);

	// again trial division, efficient enough
	for (int d = 1; d <= n; d++) {
		if (n % d == 0) {
			mpz_t term;
			mpz_init_set(term, q);
			mpz_pow_ui(term, term, d);
			int mob = mobius(n / d);
			if (mob == 1) {
				mpz_add(*result, *result, term);
			} else if (mob == -1) {
				mpz_sub(*result, *result, term);
			}
			mpz_clear(term);
		}
	}

	mpz_divexact_ui(*result, *result, n);

	return result;
}

mpz_t *choose(mpz_t set_size, int subset_size) {
	// optimized based on the assumption that subset_size << set_size
	mpz_t *result = malloc(sizeof(mpz_t));
	mpz_init_set_ui(*result, 1);
	
	mpz_t multiplier;
	mpz_init_set(multiplier, set_size);
	for (int i = 0; i < subset_size; i++) {
		mpz_mul(*result, *result, multiplier);
		mpz_sub_ui(multiplier, multiplier, 1);
	}
	mpz_clear(multiplier);

	mpz_t denom;
	mpz_init_set_ui(denom, 1);
	for (unsigned long i = 2; i <= subset_size; i++) {
		mpz_mul_ui(denom, denom, i);
	}

	mpz_divexact(*result, *result, denom);
	mpz_clear(denom);

	return result;
}

// function run by each thread in count_cycle_types
void *compute_num_cycle_types(void *void_data) {
	thread_data *data = void_data;

	for (int i = data->remainder; i < data->c.count; i += data->modulus) {
		cycle_type c = data->c.cycle_types[i];

		mpz_init_set_ui(data->results[i], 1);

		// the number of polynomials with a given cycle type (u_1,...,u_j,...,u_n) is the product of the
		// number of irreducibles of degree j choose u_j
		for (int j = 0; j < data->n; j++) {
			mpz_mul(data->results[i], data->results[i], data->chooses[j][c.vals[j]]);
		}
	}

	return NULL;
}

mpz_t *count_cycle_types(int n, mpz_t q, cycle_types c, int threads) {
	mpz_t *result = malloc(c.count * sizeof(mpz_t));

	// precompute number of irreducibles
	// irreducibles[i] is the number of degree i + 1 irreducibles
	mpz_t *irreducibles = malloc(n * sizeof(mpz_t));
	for (int i = 0; i < n; i++) {
		mpz_t *temp = num_irreducibles(i + 1, q);
		mpz_init_set(irreducibles[i], *temp);
		mpz_clear(*temp);
		free(temp);
	}

	// precompute the various choose values
	// chooses[i][j] is irreducibles[i] choose j
	mpz_t **chooses = malloc(n * sizeof(mpz_t *));
	for (int i = 0; i < n; i++) {
		chooses[i] = malloc((1 + n / (i + 1)) * sizeof(mpz_t));
		for (int j = 0; j <= n / (i + 1); j++) {
			mpz_t *temp = choose(irreducibles[i], j);
			mpz_init_set(chooses[i][j], *temp);
			mpz_clear(*temp);
			free(temp);
		}
	}

	// free irreducibles
	for (int i = 0; i < n; i++) {
		mpz_clear(irreducibles[i]);
	}
	free(irreducibles);

	// spawn threads to perform computations
	pthread_t thread_list[threads];
	thread_data *data = malloc(threads * sizeof(thread_data));
	for (int i = 0; i < threads; i++) {
		data[i].n = n;
		data[i].modulus = threads;
		data[i].remainder = i;
		data[i].c = c;
		data[i].chooses = chooses;
		data[i].results = result;

		pthread_create(&thread_list[i], NULL, compute_num_cycle_types, &data[i]);
	}

	// join threads
	for (int i = 0; i < threads; i++) {
		pthread_join(thread_list[i], NULL);
	}

	// free thread data
	free(data);

	// free precomupted data
	for (int i = 0; i < n; i++) {
		for (int j = 0; j <= n / (i + 1); j++) {
			mpz_clear(chooses[i][j]);
		}
		free(chooses[i]);
	}
	free(chooses);

	return result;
}

// test functions
#ifdef TEST_CCT

void test_mobius() {
	int vals[19] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
	int results[19] = {1, -1, -1, 0, -1, 1, -1, 0, 0, 1, 0, -1, 0, 0, 0, -1, 0, 0, 0};

	for (int i = 0; i < 19; i++) {
		assert(mobius(vals[i]) == results[i]);
	}
}

void test_num_irreducibles() {
	mpz_t thirteen, seventeen;
	mpz_init_set_ui(thirteen, 13);
	mpz_init_set_ui(seventeen, 17);

	mpz_t *test_val = num_irreducibles(1, thirteen);
	assert(!mpz_cmp_ui(*test_val, 13));
	mpz_clear(*test_val);
	free(test_val);

	test_val = num_irreducibles(2, seventeen);
	assert(!mpz_cmp_ui(*test_val, 136));
	mpz_clear(*test_val);
	free(test_val);

	mpz_clear(thirteen);
	mpz_clear(seventeen);
}

void test_count_cycle_types() {
	cycle_types *results = compute_cycle_types(3);

	mpz_t five;
	mpz_init_set_ui(five, 5);
	mpz_t *test_counts = count_cycle_types(3, five, results[3], 2);
	unsigned int correct_counts[3] = {10, 50, 40};
	for (int i = 0; i < results[3].count; i++) {
		assert(!mpz_cmp_ui(test_counts[i], correct_counts[i]));
		mpz_clear(test_counts[i]);
	}

	mpz_clear(five);
	free(test_counts);

	for (int i = 0; i <= 3; i++)
		free_cycle_types(results[i]);
	free(results);
}

int main(int argc, char **argv) {
	test_mobius();
	test_num_irreducibles();
	test_count_cycle_types();
	printf("PASS\n");
}

#endif
