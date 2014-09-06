// characters.h
// implements algorithms found in
// "Computing with Characters of the Symmetric Group" by Harm Derkson

#ifndef CHARACTERS_H
#define CHARACTERS_H

#include <gmp.h>
#include "cycle_types.h"

// returns a list containing the value of the character at each partition
// character_index is the index of the character's defining partition in the cycle_types
mpz_t *character(int n, int character_index, workspace *w);

#endif
