// characters.c
// see header file for details

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "characters.h"

// MN_rule2 returns a struct containing a list of the indices of the cycle_types that appear in the result
// and a list of their coefficients
typedef struct {
	int count;
	int *indices;
	int *coeffs;
} MN_rule2_result;

MN_rule2_result MN_rule2(partition p, int n, int k, cycle_types cs_old) {
	MN_rule2_result result;
	result.count = 0;
	int buf_size = n;
	result.indices = malloc(buf_size * sizeof(int));
	result.coeffs = malloc(buf_size * sizeof(int));

	partition u;
	for (int y = 0; y < MAX_N && p.vals[y]; y++) {
		for (int x = 0; x < p.vals[y]; x++) {
			// size of the hook with upper-left corner (x,y)
			int hook_size = p.vals[y] - x;	// initial size is the horizontal part of the hook
			for (int i = y + 1; i < MAX_N && p.vals[i] > x; i++) {
				hook_size++;
			}

			if (hook_size == k) {
				// set u to be p minus the hook specified by (x,y)
				// also compute the leg length
				int leg_length = 0;

				memset(&(u.vals), 0, sizeof(partition));

				// handle special case 
				int i;
				for (i = 0; i < y; i++) {
					u.vals[i] = p.vals[i];
				}

				while (i + 1 < MAX_N && p.vals[i] > x && p.vals[i + 1] > x) {
					u.vals[i] = p.vals[i + 1] - 1;
					i++;
					leg_length++;
				}

				u.vals[i++] = x;

				while (i + 1 < MAX_N && p.vals[i] > x && p.vals[i + 1]) {
					u.vals[i] = p.vals[i + 1] - 1;
					i++;
					leg_length++;
				}

				while (i < MAX_N && p.vals[i]) {
					u.vals[i] = p.vals[i];
					i++;
				}

				// compute the index of u and add it to result
				result.indices[result.count] = get_index(u, cs_old);
				if (leg_length % 2 == 0) {
					result.coeffs[result.count] = 1;
				} else {
					result.coeffs[result.count] = -1;
				}
				result.count++;	
			}
		}
	}

	return result;
}

// integrate with respect to the (k - 1)st variable and divide by k
mpq_t *integrate_and_divide(mpq_t *B, int n, int k, workspace *w) {
	cycle_types cs = w->results[n];
	cycle_types new_cs = w->results[n + k];

	mpq_t *result = malloc(new_cs.count * sizeof(mpq_t));

	// note that integration of terms is order-preserving
	int index = 0;
	mpq_t divisor;
	mpq_init(divisor);
	for (int new_index = 0; new_index < new_cs.count; new_index++) {
		cycle_type c = cs.cycle_types[index];
		c.vals[k - 1]++;
		mpq_set_ui(divisor, 1, c.vals[k - 1] * k);

		mpq_init(result[new_index]);
		if (!compare_cycle_types(&c, &new_cs.cycle_types[new_index])) {
			mpq_mul(result[new_index], B[index], divisor);
			index++;
		} else {
			mpq_set_ui(result[new_index], 0, 1);
		}
	}

	return result;
}

mpq_t *s_to_p(mpq_t *B, int n, int k, workspace *w) {
	if (k > n) {
		return s_to_p(B, n, n, w);
	}

	cycle_types cs = w->results[n];

	// number of cycle_types of n with max_index at most k - 1
	int count = cs.count;

	mpq_t *C = malloc(count * sizeof(mpq_t));
	for (int i = 0; i < count; i++) {
		mpq_init(C[i]);
		mpq_set_ui(C[i], 0, 1);
	}

	if (n == 0) {
		mpq_set(C[0], B[0]);
		return C;
	}

	if (k == 0) {
		mpq_set_ui(C[0], 0, 1);
		return C;
	}

	mpq_t mn_coeff;
	mpq_init(mn_coeff);
	for (int i = 0; i < cs.count; i++) {
		MN_rule2_result mn = MN_rule2(cs.partitions[i], n, k, w->results[n - k]);

		for (int j = 0; j < mn.count; j++) {
			int mn_index = mn.indices[j];
			mpq_set_si(mn_coeff, mn.coeffs[j], 1);
			mpq_mul(mn_coeff, mn_coeff, B[i]);
			mpq_add(C[mn_index], C[mn_index], mn_coeff);
		}

		free(mn.indices);
		free(mn.coeffs);
	}
	mpq_clear(mn_coeff);

	mpq_t *PC = s_to_p(C, n - k, k, w);

	// free C
	for (int i = 0; i < count; i++) {
		mpq_clear(C[i]);
	}
	free(C);

	// integrate and divide by k
	mpq_t *PC_IAD = integrate_and_divide(PC, n - k, k, w);

	// free PC
	int pc_count = w->results[n - k].count;
	for (int i = 0; i < pc_count; i++) {
		mpq_clear(PC[i]);
	}
	free(PC);

	mpq_t *PD = s_to_p(B, n, k - 1, w);

	// add PD to PC_IAD (note both have the same length)
	for (int i = 0; i < cs.count; i++) {
		mpq_add(PC_IAD[i], PC_IAD[i], PD[i]);
	}

	// free PD
	for (int i = 0; i < cs.count; i++) {
		mpq_clear(PD[i]);
	}
	free(PD);

	return PC_IAD;
}

mpz_t *character(int n, int character_index, workspace *w) {
	cycle_types cs = w->results[n];

	mpq_t *s = malloc(cs.count * sizeof(mpq_t));
	for (int i = 0; i < cs.count; i++) {
		mpq_init(s[i]);
		mpq_set_ui(s[i], 0, 1);
	}
	mpq_set_ui(s[character_index], 1, 1);

	mpq_t *p = s_to_p(s, n, n, w);
	
	// free s
	for (int i = 0; i < cs.count; i++) {
		mpq_clear(s[i]);
	}
	free(s);

	mpz_t *result = malloc(cs.count * sizeof(mpz_t));
	mpz_t multiplier;
	mpz_init(multiplier);
	mpq_t multiplier_q;
	mpq_init(multiplier_q);
	for (int i = 0; i < cs.count; i++) {
		// multiply result by denominator of class size
		cycle_type c = cs.cycle_types[i];
		mpz_set_ui(multiplier, 1);
		for (int j = 0; j < MAX_N; j++) {
			for (int k = 1; k <= c.vals[j]; k++) {
				mpz_mul_ui(multiplier, multiplier, k * (j + 1));
			}
		}
		mpq_set_z(multiplier_q, multiplier);
		mpq_mul(p[i], p[i], multiplier_q);

		mpq_canonicalize(p[i]);
		mpz_init_set(result[i], mpq_numref(p[i]));
		mpq_clear(p[i]);
	}
	free(p);
	mpz_clear(multiplier);
	mpq_clear(multiplier_q);

	return result;
}

// test functions
#ifdef TEST_CHARACTERS

// prints the character table of S_n
void print_character_table(int n) {
	workspace *w = alloc_workspace();
	compute_cycle_types(w);
	cycle_types cs = w->results[n];

	for (int i = 0; i < cs.count; i++) {
		mpz_t *row = character(n, i, w);
		for (int j = 0; j < cs.count; j++) {
			gmp_printf("%4Zd ", row[j]);
			mpz_clear(row[j]);
		}
		printf("\n");
		free(row);
	}
}

int main(int argc, char **argv) {
	if (argc > 1)
		print_character_table(atoi(argv[1]));
	else
		print_character_table(MAX_N);
}

#endif
