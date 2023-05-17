#!/bin/bash

BASEDIR=$(dirname "$(pwd)")
BENCH_DIR="$BASEDIR/bench"
PARAM_DIR="$BENCH_DIR/param"

SRC_DIR="$BASEDIR/src"
BN_DIR="$SRC_DIR/bn/amd64/ref"
FP_DIR="$SRC_DIR/fp/amd64/ref"
ECC_DIR="$SRC_DIR/ecc/amd64/ref"

DATA_DIR=$BASEDIR/data

MIN_LIMBS=1
MAX_LIMBS=10

# Record start time
start_time=$(date +%s)

# Delete previous results
rm -rf $DATA_DIR

# Create folders for benchmark data
mkdir -p $DATA_DIR
mkdir -p $DATA_DIR/sct $DATA_DIR/ct
mkdir -p $DATA_DIR/sct/bn  $DATA_DIR/ct/bn 
mkdir -p $DATA_DIR/sct/fp  $DATA_DIR/ct/fp 
mkdir -p $DATA_DIR/sct/ecc $DATA_DIR/ct/ecc

# Remove assembly files from previous runs
rm -rf $BENCH_DIR/*.s

for i in $(seq $MIN_LIMBS $MAX_LIMBS); do

    # Replace the parameters
    cp $PARAM_DIR/$i/bn_param.jinc $BN_DIR
    cp $PARAM_DIR/$i/bn_param.jinc $BN_DIR/ct

    cp $PARAM_DIR/$i/fp_param.jinc $FP_DIR
    cp $PARAM_DIR/$i/fp_param.jinc $FP_DIR/ct

    cp $PARAM_DIR/$i/ecc_param.jinc $ECC_DIR
    cp $PARAM_DIR/$i/ecc_param.jinc $ECC_DIR/ct

    # Compile (both SCT and CT)
    echo "Compiling for $i LIMBS"
    cd $BN_DIR && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $BENCH_DIR ; echo "BN SCT compiled"
    cd $BN_DIR/ct && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $BENCH_DIR ; echo "BN CT compiled"

    cd $FP_DIR && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $BENCH_DIR ; echo "FP SCT compiled"
    cd $FP_DIR/ct && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $BENCH_DIR ; echo "FP CT compiled"

    cd $ECC_DIR && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $BENCH_DIR ; echo "ECC SCT compiled"
    cd $ECC_DIR/ct && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $BENCH_DIR ; echo -e "ECC CT compiled\n"

    # Run Benchmarks
    cd $BENCH_DIR

    sed -i "s/NLIMBS [0-9]\+/NLIMBS $i/" bn_generic.c
    sed -i "s/NLIMBS [0-9]\+/NLIMBS $i/" fp_generic.c
    sed -i "s/NLIMBS [0-9]\+/NLIMBS $i/" ecc_generic.c

    # Benchmark BN SCT
    gcc cpucycles.c printbench.h bn_generic_export.s bn_generic.c -o bn.o && ./bn.o 
    mkdir -p $DATA_DIR/sct/bn/$i && mv *.csv $DATA_DIR/sct/bn/$i

    # Benchmark BN CT
    gcc cpucycles.c printbench.h bn_generic_export_ct.s bn_generic.c -o bn.o && ./bn.o 
    mkdir -p $DATA_DIR/ct/bn/$i && mv *.csv $DATA_DIR/ct/bn/$i

    # Benchmark FP SCT
    gcc cpucycles.c printbench.h fp_generic_export.s fp_generic.c -o fp.o && ./fp.o 
    mkdir -p $DATA_DIR/sct/fp/$i && mv *.csv $DATA_DIR/sct/fp/$i

    # Benchmark FP CT
    gcc cpucycles.c printbench.h fp_generic_export_ct.s fp_generic.c -o fp.o && ./fp.o 
    mkdir -p $DATA_DIR/ct/fp/$i && mv *.csv $DATA_DIR/ct/fp/$i

    # Benchmark ECC SCT
    gcc cpucycles.c printbench.h ecc_generic_export.s ecc_generic.c -o ecc.o && ./ecc.o 
    mkdir -p $DATA_DIR/sct/ecc/$i && mv *.csv $DATA_DIR/sct/ecc/$i

    # Benchmark ECC CT
    gcc cpucycles.c printbench.h ecc_generic_export_ct.s ecc_generic.c -o ecc.o && ./ecc.o 
    mkdir -p $DATA_DIR/ct/ecc/$i && mv *.csv $DATA_DIR/ct/ecc/$i
done

# Restore parameters for NLIMBS = 7
cp $PARAM_DIR/7/bn_param.jinc $BN_DIR
cp $PARAM_DIR/7/fp_param.jinc $FP_DIR
cp $PARAM_DIR/7/ecc_param.jinc $ECC_DIR

# Restore NLIMBS macro IN C files
sed -i "s/NLIMBS [0-9]\+/NLIMBS 7/" bn_generic.c
sed -i "s/NLIMBS [0-9]\+/NLIMBS 7/" fp_generic.c
sed -i "s/NLIMBS [0-9]\+/NLIMBS 7/" ecc_generic.c

# Remove object files
rm $BENCH_DIR/*.o

# Calculate and display script execution time
end_time=$(date +%s)
execution_time=$((end_time - start_time))
echo "Script execution time: $execution_time seconds"
