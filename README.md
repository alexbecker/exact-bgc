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

The files in cluster/ are intended for use on a distributed cluster where sharing memory is slow or
impossible. These are intended for use only by experienced Linux users; the following proceedure **should** work
for Ubuntu machines, but has only been tested on Ubuntu Trusty on AWS c3 instances and your machine may vary 
in ways I cannot predict. Compile the cluster versions with "make cluster MAX_N=n". 
If you have compiled anything for a different value of n, run "make clean" first. On a machine with 
network permissions appropriate for an NFS filesystem, run aws_master.sh to set it up as a master.
Replace each of the values in aws_slave.sh enclosed in percent signs with the appropriate value.
Run aws_slave.sh on each slave to connect it to the master and automatically start computing the character tables.
Once every slave has finished, run

	python combine_partial_tables.py n max_i

to combine the tables. Then run

	./evaluate_from_file n max_i filename threads prime1 prime2 ... primek

and verify the result as in the non-cluster case.

A note on performance: The parallelization works very well on machines with up to 16 cores, but on 32 cores the
overhead becomes significant. Therefore it is ideal to run on a cluster of 16 core machines.

### KNOWN BUGS:

- combine_partial_tables.py may hang on the last line of the table for some reason; if you kill it the table will be rendered faithfully.
