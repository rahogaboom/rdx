#!/bin/bash

# C build script

set -v

# pick compiler - should work with all three
#CC="clang"
CC="gcc"
#CC="g++"

# add current directory to library path for shared libraries
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

# build static library
rm -rf librdxpatc.a
$CC -c -fpic -g -ansi -pedantic -Wall -o rdx_pat_search.o rdx_pat_search.c
ar rcv librdxpatc.a rdx_pat_search.o
ar ts librdxpatc.a

# build dynamic library and set up links
rm -rf librdxpatc.so.1.0.1
$CC -shared -Wl,-soname,librdxpatc.so.1 -o librdxpatc.so.1.0.1 rdx_pat_search.o -lc
ln -sf librdxpatc.so.1.0.1 librdxpatc.so.1
ln -sf librdxpatc.so.1.0.1 librdxpatc.so

# compile test program and link with shared library
$CC -g -ansi -pedantic -Wall -o rdx_pat_test rdx_pat_test.c -L. -lrdxpatc -lm

# run test program
./rdx_pat_test

# compile gbit() test routine
$CC -o test_gbit test_gbit.c -lm

# run gbit() test routine
./test_gbit

