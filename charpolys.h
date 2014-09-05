// charpolys.h
// computes the character polynomials of irreps of S^n

#ifndef CHARPOLYS_H
#define CHARPOLYS_H

#include <gmp.h>
#include "cycle_types_and_partitions.h"

typedef struct {
	int exponents[MAX_N];
	mpq_t coeff;
} term;

typedef struct {
	int order;
	term *terms;
} polynomial;

// compute the characteristic polynomial of the irrep corresponding to c
polynomial compute_charpoly(workspace *w, cycle_type c);

// evaluate the polynomial at a set of mpz's
mpz_t *eval_polynomial_mpz(polynomial f, mpz_t *points);

#endif
