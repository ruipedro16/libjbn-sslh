#!/bin/bash

BASEDIR=$(dirname "$(pwd)")
BENCH_DIR="$BASEDIR/bench"
PARAM_DIR="$BENCH_DIR/param"

SRC_DIR="$BASEDIR/src"
BN_DIR="$SRC_DIR/bn/amd64/ref"
FP_DIR="$SRC_DIR/fp/amd64/ref"
ECC_DIR="$SRC_DIR/ecc/amd64/ref"

MIN_LIMBS=1
MAX_LIMBS=10

# To Do
for i in $(seq $MIN_LIMBS $MAX_LIMBS); do

    # Replace the parameters
    cp $PARAM_DIR/$i/bn_param.jinc $BN_DIR
    cp $PARAM_DIR/$i/fp_param.jinc $FP_DIR
    cp $PARAM_DIR/$i/ecc_param.jinc $ECC_DIR

    # Compile
    echo "Compiling for $i LIMBS"
    cd $BN_DIR && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $BENCH_DIR ; echo "BN compiled"
    cd $FP_DIR && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $BENCH_DIR ; echo "FP compiled"
    cd $ECC_DIR && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $BENCH_DIR ; echo -e "ECC compiled\n"

    # Run Benchmarks
    cd $BENCH_DIR

    sed -i "s/NLIMBS [0-9]\+/NLIMBS $i/" bn_generic.c
    sed -i "s/NLIMBS [0-9]\+/NLIMBS $i/" fp_generic.c
    sed -i "s/NLIMBS [0-9]\+/NLIMBS $i/" ecc_generic.c

    ## TODO: 
    # gcc *.s cpucycles.c printbench.h bn_generic.c -o bn.o && ./bn.o
    # gcc *.s cpucycles.c printbench.h fp_generic.c -o fp.o && ./fp.o
    # gcc *.s cpucycles.c printbench.h ecc_generic.c -o ecc.o && ./ecc.o
done

# Restore parameters for NLIMBS = 7
cp $PARAM_DIR/7/bn_param.jinc $BN_DIR
cp $PARAM_DIR/7/fp_param.jinc $FP_DIR
cp $PARAM_DIR/7/ecc_param.jinc $ECC_DIR

# Restore NLIMBS macro IN C files
sed -i "s/NLIMBS [0-9]\+/NLIMBS 7/" bn_generic.c
sed -i "s/NLIMBS [0-9]\+/NLIMBS 7/" fp_generic.c
sed -i "s/NLIMBS [0-9]\+/NLIMBS 7/" ecc_generic.c
