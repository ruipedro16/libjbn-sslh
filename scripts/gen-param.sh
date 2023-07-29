#!/bin/bash

# Validate the number of arguments
if [ $# -ne 2 ]; then
  echo "Usage: $0 <MIN_LIMBS> <MAX_LIMBS>"
  exit 1
fi

# Validate that the arguments are integers
if ! [[ $1 =~ ^[0-9]+$ ]] || ! [[ $2 =~ ^[0-9]+$ ]]; then
  echo "Error: Both arguments must be integers."
  exit 1
fi

MIN_LIMBS=$1
MAX_LIMBS=$2

gcc params.c params.h -lgmp -o params

for i in $(seq $MIN_LIMBS $MAX_LIMBS); do
  echo "$i/$MAX_LIMBS"
  nbits=$(( $i * 64 ))
  echo "Generating a $nbits bit prime"
  prime=$(openssl prime -hex -generate -bits $nbits)
  ./params j $i 0x$prime > ../bench/param/$i/fp_param.jinc
  echo -e "Generated the parameters\n"
done

rm -f params
