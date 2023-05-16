#!/bin/bash

# Tested with 7 limbs

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

rm -f *.s
(make ; echo "Compiled Fp") || (echo "Fp compilation failed" ; exit 2)
mv *.s $TEST_DIR

# Compile Ecc

cd $ECC_DIR || exit 1

# Make sure src is SCT
jasminc -checkSCT ecc_generic_export.jinc 2> /dev/null || (echo "NOT SCT" ; exit 3)

rm -f *.s
(make && echo "Compiled Ecc") || (echo "Ecc compilation failed" ; exit 2)
mv *.s $TEST_DIR

cd $TEST_DIR || exit 1

# Test Add
# gcc test_ecc_add.c *.s -o add.o
# ./add.o > /dev/null 2<&1 || (echo "Add failed" ; exit 3)

# Test Double
gcc test_ecc_double.c *.s -o double.o
(./double.o > /dev/null 2<&1 && echo "Double works" ) || (echo "Double failed" ; exit 4)

# Test Scalar Mul

# Test Mixed Add

# Test Normalize