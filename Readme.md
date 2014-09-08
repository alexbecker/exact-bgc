exact-bcg
=========

This library is designed for computing the stable representation decompositions of the cohomology groups
H^i(PConf_n(C)) of the pure braid groups. Calculations I have performed so far are located in results/.

All content in this repository is licensed under the Apache c2.0 license.

To compile the library, run the following commands:
mkdir bin
gcc -std=gnu99 evaluate.c characters.c cycle_types.c count_cycle_types.c -o bin/evaluate -DMAX_N=n -lgmp -lpthread -lm -O3
where n is the largest value of n you want to compute, usually 4*i for the largest value of i.

To calculate the representation decomposition, run:
./evaluate threads prime1 prime2 ... primek
where threads is the number of threads to use, prime1 through primek are prime numbers or prime powers.
Choose prime1 through primek to be larger than you expect any of the multiplicities of any irrep in
H^i(PConf_4i(C)) will be, and such that their product is larger than the dimension of any H^i(PConf_4i(C)).
If full_decomp_i.out.primej match for all j, this certifies the results.

TODO:
	- Add makefile
	- Optimize
	- Run on a larger machine
	- Improve documentation
