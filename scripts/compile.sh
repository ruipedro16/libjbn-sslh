#!/bin/bash

OUTDIR=$(readlink -f $1)

if [ -z "$OUTDIR" ]; then
    echo "No argument provided. Please provide a value for \$1."
    exit 1
fi

echo "Compiling Libjbn to $OUTDIR"

rm -rf $OUTDIR/*.s

BASEDIR=$(dirname "$(pwd)")
SRC_DIR="$BASEDIR/src"
BN_DIR="$SRC_DIR/bn/amd64/ref"
FP_DIR="$SRC_DIR/fp/amd64/ref"
ECC_DIR="$SRC_DIR/ecc/amd64/ref"

# Record start time
start_time=$(date +%s)

# Compile BN
echo "Compiling BN"
cd $BN_DIR
make clean > /dev/null 2>&1
make > /dev/null 2>&1 || (echo "BN failed to compile" ; exit 2)
mv bn_generic_export.s $OUTDIR

# Compile FP
echo "Compiling FP"
cd $FP_DIR
make clean > /dev/null 2>&1
make > /dev/null 2>&1 || (echo "FP failed to compile" ; exit 2)
mv fp_generic_export.s $OUTDIR

# Compile ECC
echo "Compiling ECC"
cd $ECC_DIR
make clean > /dev/null 2>&1
make > /dev/null 2>&1 || (echo "ECC failed to compile" ; exit 2)
mv ecc_generic_export.s $OUTDIR

# Calculate and display compilation time
end_time=$(date +%s)
compilation_time=$((end_time - start_time))
echo "Compilation took approx. $compilation_time seconds"
