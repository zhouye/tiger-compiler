#!/bin/bash
llvm-as $1.ll
llc $1.bc
rm $1.bc
cat runtime.s >> $1.gcc.s
cat $1.s >> $1.gcc.s
mv $1.gcc.s $1.s
gcc $1.s -o $1 -O2

