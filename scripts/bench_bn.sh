#!/bin/bash

BASEDIR=$(dirname "$(pwd)")
BENCH_DIR="$BASEDIR/bench"
ASM_DIR="$BENCH_DIR/asm"

PARAM_DIR="$BENCH_DIR/param"

SRC_DIR="$BASEDIR/src"
BN_DIR="$SRC_DIR/bn/amd64/ref"

DATA_DIR="$BASEDIR/data_bn"

CFLAGS="-Wall -Wno-unused-function -Wno-unused-variable"

# Check if $1 and $2 are provided
if [ -z "$1" ] || [ -z "$2" ]; then
    echo "Error: Both arguments (MIN_LIMBS & MAX_LIMBS) must be provided."
    exit 1
fi

# Check if $1 is a number and greater > 0
if ! [[ $1 =~ ^[0-9]+$ ]] || [ "$1" -le 0 ]; then
    echo "Error: \$1 must be a positive integer."
    exit 1
fi

if ! [[ $2 =~ ^[0-9]+$ ]] || [ "$2" -le "$1" ]; then
    echo "Error: \$2 must be a number greater than \$1."
    exit 1
fi

MIN_LIMBS=$1
MAX_LIMBS=$2

mkdir -p $ASM_DIR

# Delete previous results
rm -rf $DATA_DIR

# Create folders for benchmark data
mkdir -p $DATA_DIR/sct $DATA_DIR/ct $DATA_DIR/tmp
mkdir -p $DATA_DIR/sct/bn  $DATA_DIR/ct/bn

for i in $(seq $MIN_LIMBS $MAX_LIMBS); do
    mkdir -p $ASM_DIR/$i/ct
    mkdir -p $ASM_DIR/$i/sct
    
    echo "Compiling $i/$2 LIMBS"

    # Replace the parameters
    # echo "param int NLIMBS=$i;" > $BN_DIR/bn_param.jinc
    # echo "param int NLIMBS=$i;" > $BN_DIR/ct/bn_param.jinc

    # Compile (both SCT and CT)
    # cd $BN_DIR && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $ASM_DIR/$i/sct ; echo "BN SCT compiled"
    # cd $BN_DIR/ct && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $ASM_DIR/$i/ct ; echo "BN CT compiled"

    # Bench 
    cd $BENCH_DIR || exit 1

    # Benchmark BN SCT
    cp $ASM_DIR/$i/sct/* $BENCH_DIR
    gcc $CFLAGS cpucycles.c printbench.h bn_generic_export.s bn_generic.c -o bn.o 
    echo "Running Benchmarks: BN SCT" && ./bn.o
    mkdir -p $DATA_DIR/sct/bn/$i && mv *.csv $DATA_DIR/sct/bn/$i
    echo "Function,SCT" > $DATA_DIR/tmp/merged.csv                # merge all files
    cat $DATA_DIR/sct/bn/$i/bn_*.csv >> $DATA_DIR/tmp/merged.csv  # merge all files
    mv $DATA_DIR/tmp/merged.csv $DATA_DIR/sct/bn/$i/merged.csv    # merge all files

    # Benchmark BN CT
    cp $ASM_DIR/$i/ct/* $BENCH_DIR
    gcc $CFLAGS cpucycles.c printbench.h bn_generic_export_ct.s bn_generic.c -o bn.o 
    echo -e "Running Benchmarks: BN CT\n" && ./bn.o
    mkdir -p $DATA_DIR/ct/bn/$i && mv *.csv $DATA_DIR/ct/bn/$i
    echo "Function,CT" > $DATA_DIR/tmp/merged.csv                # merge all files
    cat $DATA_DIR/ct/bn/$i/bn_*.csv >> $DATA_DIR/tmp/merged.csv  # merge all files
    mv $DATA_DIR/tmp/merged.csv $DATA_DIR/ct/bn/$i/merged.csv    # merge all files
done

# Restore parameters for NLIMBS = 4
cp $PARAM_DIR/4/bn_param.jinc $BN_DIR
cp $PARAM_DIR/4/bn_param.jinc $BN_DIR/ct

# Restore NLIMBS macro in C files
sed -i "s/NLIMBS [0-9]\+/NLIMBS 7/" bn_generic.c
