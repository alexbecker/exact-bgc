exact-bgc
=========

Copyright 2014 Alex Becker

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

This license applies to all code in this repository.

This library is designed for computing the stable representation decompositions of the cohomology groups
H^i(PConf_n(C)) of the pure braid groups. Calculations I have performed so far are located in results/.

To compile the library, run "make MAX_N=n" where n is the largest value of n you want to work with,
usually 4 times the largest value of i for which you want to compute the stable decomposition of
H^i(PConf_n(C)).

To calculate the representation decomposition, run:

./evaluate n max_i threads prime1 prime2 ... primek

where threads is the number of threads to use, prime1 through primek are prime numbers or prime powers.
This will produce the decompositions for i up to max_i.
Choose prime1 through primek to be larger than you expect any of the multiplicities of any irrep in
H^i(PConf_n(C)) will be, and such that their product is larger than the dimension of any H^i(PConf_n(C)).
If full_decomp.out.primej match for all j, this certifies the results.

TODO:

	- Optimize

	- Run on a larger machine

	- Improve documentation
