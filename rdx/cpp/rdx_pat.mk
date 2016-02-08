#!/bin/bash

# CPP build script

set -v

if [ "$1" == 'clean' ]
then
    rm -f rdx_pat_test
    rm -f MKRdxPat-test.results
    rm -f rdx_pat_perf
    exit
fi

# pick compiler - should work with both
CC="clang"
#CC="g++"

# compile test program - since the entire rdx class is in a header
# file(MKRdxPat.h), there is no seperate class compile
$CC -std=c++14 -g -pedantic -Wall -o rdx_pat_test rdx_pat_test.cpp -lstdc++

# run test program
./rdx_pat_test

# compile perf program - future release
#$CC -std=c++14 -g -pedantic -Wall -o rdx_pat_perf rdx_pat_perf.cpp -lm

