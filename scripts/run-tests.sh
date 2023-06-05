#!/bin/bash

# Tested with 7 limbs

# Exit codes:
# 1: cd failed
# 2: jasminc -checkSCT failed
# 3: jasminc failed
# 4: test (C) failed

BASEDIR=$(dirname "$(pwd)")
TEST_DIR="$BASEDIR/test"
SRC_DIR="$BASEDIR/src"
FP_DIR="$SRC_DIR/fp/amd64/ref"
ECC_DIR="$SRC_DIR/ecc/amd64/ref"

# Remove existing assembly files from test directory
rm -f $TEST_DIR/*.s

# Compile Fp
cd $FP_DIR || exit 1

# Make sure src is SCT
jasminc -checkSCT fp_generic_export.jinc 2> /dev/null || (echo "NOT SCT" ; exit 2)

rm -f *.s # same as make clean
(make > /dev/null 2>&1 ; echo "Compiled Fp") || (echo "Fp compilation failed" ; exit 3)
mv *.s $TEST_DIR

# Compile Ecc
cd $ECC_DIR || exit 1

# Make sure src is SCT
jasminc -checkSCT ecc_generic_export.jinc 2> /dev/null || (echo "NOT SCT" ; exit 2)

rm -f *.s # same as make clean
(make > /dev/null 2>&1 && echo -e "Compiled Ecc\n") || (echo "Ecc compilation failed" ; exit 3)
mv *.s $TEST_DIR

cd $TEST_DIR || exit 1

# Test Add
gcc test_ecc_add.c *.s -o add.o
(./add.o > /dev/null 2>&1 && echo "Add works") || (echo "\033[31mAdd failed\033[0m" ; exit 4)

# Test Double
gcc test_ecc_double.c *.s -o double.o
(./double.o > /dev/null 2>&1 && echo "Double works" ) || (echo "\033[31mDouble failed\033[0m" ; exit 4)

# Test Scalar Mul
gcc test_ecc_scalar_mult.c *.s -o scalar_mul.o
(./scalar_mul.o > /dev/null 2>&1 && echo "Scalar Mult works" ) || (echo "\033[31mScalar Mult failed\033[0m" ; exit 4)

# Test Branchless Scalar Mul
gcc test_ecc_scalar_mult_branchless.c *.s -o scalar_mult_branchless.o
(./scalar_mult_branchless.o > /dev/null 2>&1 && echo "Branchless Scalar Mult works" ) || (echo "\033[31mBranchless Scalar Mult failed\033[0m" ; exit 4)

# Test Mixed Add
gcc test_ecc_mixed_add.c *.s -o mixed_add.o
(./mixed_add.o > /dev/null 2>&1 && echo "Mixed Add works" ) || (echo "\033[31mMixed Add failed\033[0m" ; exit 4)

# Test Normalize
gcc test_ecc_normalize.c *.s -o normalize.o
(./normalize.o > /dev/null 2>&1 && echo "Normalize works") || (echo "\033[31mNormalize failed\033[0m" ; exit 4)
