#!/bin/bash

BASEDIR=$(dirname "$(pwd)")
BENCH_DIR="$BASEDIR/bench"
ASM_DIR="$BENCH_DIR/asm"

PARAM_DIR="$BENCH_DIR/param"

SRC_DIR="$BASEDIR/src"
BN_DIR="$SRC_DIR/bn/amd64/ref"
FP_DIR="$SRC_DIR/fp/amd64/ref"
ECC_DIR="$SRC_DIR/ecc/amd64/ref"

MIN_LIMBS=1
MAX_LIMBS=32

rm -rf $ASM_DIR # delete previously compiled files

mkdir -p $ASM_DIR

cd $ASM_DIR || exit 1

for i in $(seq $MIN_LIMBS $MAX_LIMBS); do
    mkdir -p $ASM_DIR/$i/ct
    mkdir -p $ASM_DIR/$i/sct

    # Replace the parameters
    cp $PARAM_DIR/$i/bn_param.jinc $BN_DIR
    cp $PARAM_DIR/$i/bn_param.jinc $BN_DIR/ct
    
    cp $PARAM_DIR/$i/fp_param.jinc $FP_DIR
    cp $PARAM_DIR/$i/fp_param.jinc $FP_DIR/ct
    
    cp $PARAM_DIR/$i/ecc_param.jinc $ECC_DIR
    cp $PARAM_DIR/$i/ecc_param.jinc $ECC_DIR/ct

    echo "Compiling for $i LIMBS\n"

    # Compile (both SCT and CT)
    cd $BN_DIR && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $ASM_DIR/$i/sct ; echo "BN SCT compiled"
    cd $BN_DIR/ct && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $ASM_DIR/$i/ct ; echo "BN CT compiled"

    cd $FP_DIR && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $ASM_DIR/$i/sct ; echo "FP SCT compiled"
    cd $FP_DIR/ct && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $ASM_DIR/$i/ct ; echo "FP CT compiled"
    
    cd $ECC_DIR && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $ASM_DIR/$i/sct ; echo "ECC SCT compiled"
    cd $ECC_DIR/ct && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $ASM_DIR/$i/ct ; echo "ECC CT compiled\n"

done

# Restore parameters for NLIMBS = 7
cp $PARAM_DIR/7/bn_param.jinc $BN_DIR
cp $PARAM_DIR/7/bn_param.jinc $BN_DIR/ct

cp $PARAM_DIR/7/fp_param.jinc $FP_DIR
cp $PARAM_DIR/7/fp_param.jinc $FP_DIR/ct

cp $PARAM_DIR/7/ecc_param.jinc $ECC_DIR
cp $PARAM_DIR/7/ecc_param.jinc $ECC_DIR/ct

