// charpolys.c
// see header file for details
// implements waaaaay more than it exports

#include <stdlib.h>
#include <stdio.h>
#include "charpolys.h"

// NOTE: must be a power of 2 - 1
#define MPQ_CANONICALIZE_INTERVAL 255

// we use the lexigraphical ordering on the monomials
int compare_terms(const void *a_void, const void *b_void) {
	term *a = a_void;
	term *b = b_void;
	for (int i = 0; i < MAX_N; i++) {
		if (a.exponents[i] < b.exponents[i]) {
			return -1;
		} else if (a.exponents[i] > b.exponents[i]) {
			return 1;
		}
	}

	return 0;
}

void compress_polynomial(polynomial *f) {
	qsort(f->terms, f->order, sizeof(term), compare_terms);

	term *new_terms = malloc(f->order * sizeof(term));
	mpq_init(new_terms[0].coeff);
	mpq_set_si(new_terms[0].coeff, terms[0].coeff);
	int index = 0, next_i;
	for (int i = 0; i < f->order - 1; i = next_i) {
		next_i = i + 1;
		of (compare_terms(terms[i], terms[next_i])) {
			index++;
			new_terms[index].exponents = terms[next_i].exponents;
			mpq_init(new_terms[index].coeff);
			mpq_set(new_terms[index].coeff, terms[next_i].coeff);
		} else {
			mpq_add(new_terms[index], terms[next_i].coeff);
		}
	}

	new_terms = realloc(new_terms, (index + 1) * sizeof(term));

	free(f->terms);
	f->terms = new_terms;
}

mpz_t *mpz_mono_value(int dimension, int num_points, term t, mpz_t *points) {
	mpz_t *result = malloc(num_points * sizeof(mpz_t));

	for (int i = 0; i < num_points; i++) {
		mpz_init_set_si(result[i], 1);

		for (int j = 0; j < dimension; j++) {
			mpz_t multiplier;
			mpz_init(multiplier);

			// note: cache inefficiency is minimal since dimension is small
			mpz_pow_ui(multiplier, points[i * dimension + j], t.exponents[j]);

			mpz_mul(result[i], result[i], multiplier);
		}
	}

	return result;
}

// note that even though the polynomial has non-integer coefficients,
// it takes only integer values
mpz_t *eval_polynomial_cycle_types(polynomial f, cycle_types cs) {
	mpq_t *rat_result = malloc(cs.count * sizeof(mpq_t));
	mpz_t *result = malloc(cs.count * sizeof(mpz_t));
	mpq_t temp;
	mpq_init(temp);

	for (int i = 0; i < cs.count; i++) {
		mpq_init(rat_result[i]);
		mpq_set_si(rat_result[i], 0, 1);
	}

	for (int i = 0; i < f.order; i++) {
		int *monomial_values = mono_value(f.dimension, cs.count, f.terms[i], cs.cycle_types);

		for (int j = 0; j < cd.count; j++) {
			mpq_set_si(temp, monomial_values[j]);
			mpq_mul(temp, temp, f.coeffs[i]);
			mpq_add(rat_result[j], rat_result[j], temp);
		}

		free(monomial);
		free(monomial_values);

		// canonicalize periodically
		if (i & MPQ_CANONICALIZE_INTERVAL == 0 && i > 0) {
			for (int j = 0; j < cd.count; j++) {
				mpq_canonicalize(rat_result[j]);
			}
		}
	}

	mpq_clear(temp);

	for (int i = 0; i < cs.count; i++) {
		mpq_canonicalize(rat_result[i]);
		mpz_init_set(result[i], mpq_numref(rat_result[i]));
		mpq_clear(rat_result[i]);
	}

	free(rat_result);

	return result;
}

mpz_t *eval_polynomial

polynomial zero(int dimension) {
	polynomial result;
	result.order = 1;
	result.dimension = dimension;
	result.monomials = malloc(sizeof(int));
	result.monomials[0] = 1;
	result.coeffs = malloc(sizeof(mpq_t));
	mpq_init(result.coeffs[0]);
	mpq_set_si(result.coeffs[0], 0);

	return result;
}

polynomial one(int dimension) {
	polynomial result;
	result.order = 1;
	result.dimension = dimension;
	result.monomials = malloc(sizeof(int));
	result.monomials[0] = 1;
	result.coeffs = malloc(sizeof(mpq_t));
	mpq_init(result.coeffs[0]);
	mpq_set_si(result.coeffs[0], 1);

	return result;
}

void free_polynomial(polynomial f) {
	free(f.monomials);
	for (int i = 0; i < f.order; f++) {
		mpq_clear(f.coeffs[i]);
	}
	free(f.coeffs);
}

// WARNING: all operations assume dimension is the same!
// TODO: fix ordering
polynomial add(polynomial f, polynomial g) {
	assert(f.dimension == g.dimension);

	polynomial result;
	result.monomials = malloc((f.order + g.order) * sizeof(int));
	result.coeffs = malloc((f.order + g.order) * sizeof(mpq_t));

	int f_index = 0, g_index = 0, result_index = 0;
	while (f_index < f.order && g_index < g.order) {
		if (f_index < g_index) {
			result.monomials[result_index] = f.monomials[f_index];
			mpq_init(result.coeffs[result_index]);
			mpq_set(result.coeffs[result_index], f.coeffs[f_index]);
			f_index++;
		} else if (f_index > g_index) {
			result.monomials[result_index] = g.monomials[g_index];
			mpq_init(result.coeffs[result_index]);
			mpq_set(result.coeffs[result_index], g.coeffs[g_index]);
			g_index++;
		} else {
			result.monomials[result_index] = f.monomials[f_index];
			mpq_init(result.coeffs[result_index]);
			mpq_set(result.coeffs[result_index], f.coeffs[f_index]);
			mpq_add(result.coeffs[result_index], g.coeffs[g_index]);
			f_index++;
			g_index++;
		}

		result_index++;
	}

	while (f_index < f.order) {
		result.monomials[result_index] = f.monomials[f_index];
		mpq_init(result.coeffs[result_index]);
		mpq_set(result.coeffs[result_index], f.coeffs[f_index]);
		f_index++;
		result_index++;
	}
		
	while (g_index < g.order) {
		result.monomials[result_index] = g.monomials[g_index];
		mpq_init(result.coeffs[result_index]);
		mpq_set(result.coeffs[result_index], g.coeffs[g_index]);
		g_index++;
		result_index++;
	}

	result.coeffs = realloc(result.coeffs, result_index * sizeof(mpq_t));

	return result;
}
