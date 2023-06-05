#!/bin/bash

BASEDIR=$(dirname "$(pwd)")
BENCH_DIR="$BASEDIR/bench"
ASM_DIR="$BENCH_DIR/asm"

PARAM_DIR="$BENCH_DIR/param"

SRC_DIR="$BASEDIR/src"
BN_DIR="$SRC_DIR/bn/amd64/ref"
FP_DIR="$SRC_DIR/fp/amd64/ref"
ECC_DIR="$SRC_DIR/ecc/amd64/ref"

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

# Check if $2 is a number and <= 12
if ! [[ $2 =~ ^[0-9]+$ ]] || [ "$2" -gt 32 ]; then
    echo "Error: \$2 must be an integer <= 12."
    exit 1
fi

MIN_LIMBS=$1
MAX_LIMBS=$2

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
    
    echo -e "Compiling for $i LIMBS\n"
    
    # Compile (both SCT and CT)
    cd $BN_DIR && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $ASM_DIR/$i/sct ; echo "BN SCT compiled"
    cd $BN_DIR/ct && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $ASM_DIR/$i/ct ; echo "BN CT compiled"
    
    cd $FP_DIR && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $ASM_DIR/$i/sct ; echo "FP SCT compiled"
    cd $FP_DIR/ct && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 && mv *.s $ASM_DIR/$i/ct ; echo "FP CT compiled"
    
    if [[ $i -gt 13 ]]; then # if NLIMBS > 13, ECC doesnt compile (Fatal error: exception Stack overflow)
        echo -e "Skiping ECC\n"
    else
        cd $ECC_DIR && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 ; mv *.s $ASM_DIR/$i/sct ; echo "ECC SCT compiled"        
        cd $ECC_DIR/ct && make clean > /dev/null 2>&1 ; make > /dev/null 2>&1 ; mv *.s $ASM_DIR/$i/sct ; echo -e "ECC CT compiled\n"
    fi
    
done

# Restore parameters for NLIMBS = 4
cp $PARAM_DIR/4/bn_param.jinc $BN_DIR
cp $PARAM_DIR/4/bn_param.jinc $BN_DIR/ct

cp $PARAM_DIR/4/fp_param.jinc $FP_DIR
cp $PARAM_DIR/4/fp_param.jinc $FP_DIR/ct

cp $PARAM_DIR/4/ecc_param.jinc $ECC_DIR
cp $PARAM_DIR/4/ecc_param.jinc $ECC_DIR/ct
