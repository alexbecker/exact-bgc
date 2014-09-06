// characters.c
// see header file for details

#include <stdlib.h>
#include "characters.c"

// MN_rule2 returns a struct containing a list of the indices of the cycle_types that appear in the result
// and a list of their coefficients
typedef struct {
	int count;
	int *indices;
	int *coeffs;
} MN_rule2_result;

MN_rule2_result MN_rule2(cycle_type c, int n, int k, cycle_types cs) {
	MN_rule2_result result;
	result.count = 0;
	int buf_size = n;
	result.indices = malloc(buf_size * sizeof(int));
	result.coeffs = malloc(buf_size * sizeof(int));

	cycle_type u = c;
	int i = 0;

	for (int j = 0; j < n; j++) {
		if (k + c.vals[j + 1] + i <= c.vals[i] + j) {
			if (k + c.vals[j] + i > c.vals[i] + j) {
				u[j] = c.vals[i] + j - i - k;
			
				if (result.count >= buf_size) {
					buf_size *= 2;
					result.indices = realloc(result.indices, buf_size * sizeof(int));
					result.coeffs = realloc(result.coeffs, buf_size * sizeof(int));
				}

				result.indices[result.count] = get_index(u, cs);
				if ((i - j) % 2 == 0) {
					result.coeffs[result.count] = 1;
				} else {
					result.coeffs[result.count] = -1;
				}
				result.count++;
			}
			i++;
			u[i - 1] = c.vals[i - 1];
		} else {
			j++;
			if (j > i) {
				u[j - 1] = c.vals[j] - 1;
			}
		}
	}

	return result;
}

// integrate with respect to the kth variable and divide by k
mpq_t *integrate_and_divide(mpz_t *B, int n, int k, workspace *w) {
	mpq_t *result = malloc(cs.count * sizeof(mpq_t));

	cycle_type cs = w->results[n];
	cycle_type new_cs = w->results[n + k];

	// note that integration of terms is order-preserving
	int index = 0;
	mpq_t divisor;
	mpq_init(divisor);
	mpq_set_ui(divisor, 1, k * (k + 1));
	for (int new_index = 0; new_index < new_cs.count; new_index++) {
		cycle_type c = cs.cycle_types[index];
		c[k]++;

		mpq_init(result[new_index]);
		if (!compare_cycle_types(&c, &new_cs.cycle_types[new_index])) {
			mpq_mul(result[new_index], B[index], divisor);
			index++;
		} else {
			mpq_set_ui(result[new_index], 0, 1);
		}
	}
}

mpq_t *s_to_p(mpq_t *B, int n, int k, workspace *w) {
	if (k > n) {
		return s_to_p(B, n, n, w);
	}

	cycle_types cs = w->results[n];

	// number of cycle_types of n with max_index at most k - 1
	int count;
	if (k == n) {
		count = cs.count;
	} else {
		count = cs.start_positions[k];
	}

	mpq_t *C = malloc(count * sizeof(mpq_t));
	for (int i = 0; i < count; i++) {
		mpq_init(C[i]);
		mpq_set_ui(C[i], 1, 0);
	}

	if (n == 0) {
		mpq_set(C[0], B[0]);
		return C;
	}

	if (k == 0) {
		return C;
	}

	mpq_t mn_coeff;
	mpq_init(mn_coeff);
	for (int i = 0; i < cs.count; i++) {
		MN_rule2_result mn = MN_rule2(cs.cycle_types[i], n, k, w);

		for (int j = 0; j < n; j++) {
			int mn_index = mn.indices[j];
			mpq_set_si(mn_coeff, mn.coeffs[j], 1);
			mpq_mul(mn_coeff, mn_coeff, B[mn_index]);
			mpq_add(C[mn_index], C[mn_index], mn_coeff);
		}
	}
	mn_clear(mn_coeff);

	mpq_t *PC = s_to_p(C, n - k, k, w);

	// free C
	for (int i = 0; i < count; i++) {
		mpq_clear(C[i]);
	}
	free(C);

	// integrate and divide by k
	mpq_t *PC_IAD = integrate_and_divide(PC, n - k, k, w);

	// free PC
	for (int i = 0; i < count; i++) {
		mpq_clear(PC[i]);
	}
	free(PC);

	mpq_t *PD = s_to_p(B, n, k - 1, w);

	// add PD to PC_IAD (note both have the same length)
	for (int i = 0; i < cs.count; i++) {
		mpq_add(PC_IAD[i], PC_IAD[i], PD[i]);
	}

	// free PD
	for (int i = 0; i < count; i++) {
		mpq_clear(PD[i]);
	}
	free(PD);

	return PC_IAD;
}

// TODO: implement
mpz_t *characters(int n, int character_index, workspace *w)
