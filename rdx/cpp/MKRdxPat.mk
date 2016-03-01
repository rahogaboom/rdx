#!/bin/bash

# CPP build script

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

# compile test program - using MKRdxPat.hpp
$CC -std=c++14 -g -pedantic -Wall -o MKRdxPat_test MKRdxPat_test.cpp -lstdc++
# $CC -std=c++14 -O -pedantic -Wall -o MKRdxPat_test MKRdxPat_test.cpp -lstdc++

# run test program
./MKRdxPat_test

# compile perf program
$CC -std=c++14 -O -pedantic -Wall -o MKRdxPat_perf MKRdxPat_perf.cpp -lstdc++

