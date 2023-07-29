#!/bin/bash

BASEDIR=$(dirname "$(pwd)")
BENCH_DIR="$BASEDIR/bench"
ASM_DIR="$BENCH_DIR/asm"
DATA_DIR=$BASEDIR/data
PARAM_DIR="$BENCH_DIR/param"

MIN_LIMBS=1
MAX_LIMBS=12

CFLAGS="-Wall -Wno-unused-function -Wno-unused-variable" #-Ofast -march=native -fomit-frame-pointer"

# Delete previous results
rm -rf $DATA_DIR

# Create folders for benchmark data
mkdir -p $DATA_DIR/sct $DATA_DIR/ct $DATA_DIR/tmp
mkdir -p $DATA_DIR/sct/bn  $DATA_DIR/ct/bn
mkdir -p $DATA_DIR/sct/fp  $DATA_DIR/ct/fp
mkdir -p $DATA_DIR/sct/ecc $DATA_DIR/ct/ecc

cd $BENCH_DIR || exit 1

for i in $(seq $MIN_LIMBS $MAX_LIMBS); do

    echo "$i Limbs"

    sed -i "s/NLIMBS [0-9]\+/NLIMBS $i/" bn_generic.c
    sed -i "s/NLIMBS [0-9]\+/NLIMBS $i/" fp_generic.c
    sed -i "s/NLIMBS [0-9]\+/NLIMBS $i/" ecc_generic.c

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
    echo "Running Benchmarks: BN CT" && ./bn.o
    mkdir -p $DATA_DIR/ct/bn/$i && mv *.csv $DATA_DIR/ct/bn/$i
    echo "Function,CT" > $DATA_DIR/tmp/merged.csv                # merge all files
    cat $DATA_DIR/ct/bn/$i/bn_*.csv >> $DATA_DIR/tmp/merged.csv  # merge all files
    mv $DATA_DIR/tmp/merged.csv $DATA_DIR/ct/bn/$i/merged.csv    # merge all files

    # Benchmark FP SCT
    cp $ASM_DIR/$i/sct/* $BENCH_DIR
    gcc $CFLAGS cpucycles.c printbench.h fp_generic_export.s fp_generic.c -o fp.o 
    echo "Running Benchmarks: FP SCT" && ./fp.o
    mkdir -p $DATA_DIR/sct/fp/$i && mv *.csv $DATA_DIR/sct/fp/$i
    echo "Function,SCT" > $DATA_DIR/tmp/merged.csv                # merge all files
    cat $DATA_DIR/sct/fp/$i/fp_*.csv >> $DATA_DIR/tmp/merged.csv  # merge all files
    mv $DATA_DIR/tmp/merged.csv $DATA_DIR/sct/fp/$i/merged.csv    # merge all files
    
    # Benchmark FP CT
    cp $ASM_DIR/$i/ct/* $BENCH_DIR
    gcc $CFLAGS cpucycles.c printbench.h fp_generic_export_ct.s fp_generic.c -o fp.o 
    echo "Running Benchmarks: FP CT" && ./fp.o
    mkdir -p $DATA_DIR/ct/fp/$i && mv *.csv $DATA_DIR/ct/fp/$i
    echo "Function,CT" > $DATA_DIR/tmp/merged.csv                # merge all files
    cat $DATA_DIR/ct/fp/$i/fp_*.csv >> $DATA_DIR/tmp/merged.csv  # merge all files
    mv $DATA_DIR/tmp/merged.csv $DATA_DIR/ct/fp/$i/merged.csv    # merge all files

    # Benchmark ECC SCT
    cp $ASM_DIR/$i/sct/* $BENCH_DIR
    gcc $CFLAGS cpucycles.c printbench.h ecc_generic_export.s ecc_generic.c -o ecc.o 
    echo "Running Benchmarks: ECC SCT" && ./ecc.o
    mkdir -p $DATA_DIR/sct/ecc/$i && mv *.csv $DATA_DIR/sct/ecc/$i
    echo "Function,SCT" > $DATA_DIR/tmp/merged.csv                  # merge all files
    cat $DATA_DIR/sct/ecc/$i/ecc_*.csv >> $DATA_DIR/tmp/merged.csv  # merge all files
    mv $DATA_DIR/tmp/merged.csv $DATA_DIR/sct/ecc/$i/merged.csv     # merge all files
    
    # Benchmark ECC CT
    cp $ASM_DIR/$i/ct/* $BENCH_DIR
    gcc $CFLAGS cpucycles.c printbench.h ecc_generic_export_ct.s ecc_generic.c -o ecc.o 
    echo -e "Running Benchmarks: ECC CT\n\n" && ./ecc.o
    mkdir -p $DATA_DIR/ct/ecc/$i && mv *.csv $DATA_DIR/ct/ecc/$i
    echo "Function,CT" > $DATA_DIR/tmp/merged.csv                  # merge all files
    cat $DATA_DIR/ct/ecc/$i/ecc_*.csv >> $DATA_DIR/tmp/merged.csv  # merge all files
    mv $DATA_DIR/tmp/merged.csv $DATA_DIR/ct/ecc/$i/merged.csv     # merge all files

done

# Restore NLIMBS macro in C files
sed -i "s/NLIMBS [0-9]\+/NLIMBS 7/" bn_generic.c
sed -i "s/NLIMBS [0-9]\+/NLIMBS 7/" fp_generic.c
sed -i "s/NLIMBS [0-9]\+/NLIMBS 7/" ecc_generic.c

# Remove object files
rm $BENCH_DIR/*.o

# Remove tmp directory
rm -rf $DATA_DIR/tmp
