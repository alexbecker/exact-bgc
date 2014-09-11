// multiplicities.c
// calculates the multiplicities of irreps in H^i(PConf_n(C))

#include <stdlib.h>
#include <stdio.h>
#include "multiplicities.h"

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
