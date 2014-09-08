// count_cycle_types.h
// counts the number of squarefree polynomials with a given cycle type

#ifndef COUNT_CYCLE_TYPES_H
#define COUNT_CYCLE_TYPES_H

#include <gmp.h>
#include "cycle_types.h"

mpz_t *count_cycle_types(int n, long q, cycle_types c, int threads);

#endif

