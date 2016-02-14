#!/bin/bash

# CPP build script

set -v

if [ "$1" == 'clean' ]
then
    rm -f MKrdxPat_test
    rm -f MKRdxPat.TEST?.results
    rm -f MKRdxPat.TEST??.results
    rm -f MKRdxPat_perf
    exit
fi

# pick compiler - should work with both
CC="clang"
#CC="g++"

# compile test program - since the entire rdx class is in a header
# file(MKRdxPat.h), there is no seperate class compile
$CC -std=c++14 -g -pedantic -Wall -o MKRdxPat_test MKRdxPat_test.cpp -lstdc++

# run test program
./MKRdxPat_test

# compile perf program - future release
#$CC -std=c++14 -g -pedantic -Wall -o MKRdxPat_perf MKRdxPat_perf.cpp -lm

