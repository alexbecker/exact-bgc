// multiplicities.h
// calculates the multiplicities of irreps in H^i(PConf_n(C))

#ifndef MULTIPLICITIES_H
#define MULTIPLICITIES_H

#include <stdio.h>
#include <gmp.h>
#include "cycle_types.h"

// get the multiplicities mod q of a character in H^i(PConf(C^n)) for all i
mpz_t *get_multiplicities(int n, mpz_t sum, mpz_t q);

// prints the partition corresponding to the character, and the multiplicity, in readable format
void print_character_multiplicity(FILE *fp, partition p, mpz_t multiplicity);

#endif
