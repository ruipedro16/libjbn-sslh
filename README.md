# libjbn-slh

- Scripts
    - [scalar_mul.py](scripts/scalar_mul.py): Python script for scalar multiplication 
    using the Montgomery Ladder

    - [run-tests.sh](scripts/run-tests.sh): Script to make sure that the jasmin implementation 
    outputs correct results (for 7 limbs only)

    - [gen-bench.sh](scripts/gen-bench.sh): Script to generate benchmark results (for NLIMBS
     in [1, 10]). Results are stored in the `data` directory. Running this scripts takes
     approx. 20 min

    - [compile.sh](scripts/compile.sh): Compile the library and output the assembly files to the 
    folder specified in `$1`. For 7 limbs, compilation takes approx. 2 minutes
