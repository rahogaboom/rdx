#!/bin/bash

# C++ build script

set -v

if [ "$1" == 'clean' ]
then
    rm -f MKRdxPat_test
    rm -f MKRdxPat.TEST?.results
    rm -f MKRdxPat.TEST??.results
    rm -f MKRdxPat_perf
    rm -f MKRdxPat_perf.results
    exit
fi

# pick compiler
CC="clang"
#CC="g++"

# pick language dialect
#LD="c++11"
#LD="c++14"
#LD="gnu++11"
LD="gnu++14"

# pick debug/optimize
O=g
#O=O

# compile test program - using MKRdxPat.hpp
$CC -std=$LD -$O -pedantic -Wall -o MKRdxPat_test MKRdxPat_test.cpp -lstdc++

# run test program
./MKRdxPat_test

# compile perf program
$CC -std=$LD -O -pedantic -Wall -o MKRdxPat_perf MKRdxPat_perf.cpp -lstdc++

