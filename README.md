# libjbn-sslh

## Scripts

- [bench_bn.sh](scripts/bench_bn.sh): Script to generate benchmark results for only integer arithmetic  
(for NLIMBS between `$1` and `$2`). Results are stored in the `data_bn` directory.

- [compile-all-limbs.sh](scripts/compile-all-limbs.sh): Script to compile the library for the
number of limbs between `$1` and `$2`.

- [compile.sh](scripts/compile.sh): Compile the library and output the assembly files to the
folder specified in `$1`. For 7 limbs, compilation takes approx. 2 minutes

- [run-bench.sh](scripts/run-bench.sh): Script to generate benchmark results (for NLIMBS
in [1, 12]). Results are stored in the `data` directory.

- [run-tests.sh](scripts/run-tests.sh): Script to make sure that the Jasmin implementation
outputs correct results (for 7 limbs only)

- [scalar_mul.py](scripts/scalar_mul.py): Python script for scalar multiplication
using the Montgomery Ladder
