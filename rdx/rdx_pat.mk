#!/bin/bash

# C build script

set -v

# add current directory to library path for shared libraries
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

# build static library
rm -rf librdxpatc.a
gcc -c -fpic -g -ansi -pedantic -Wall -o rdx_pat_search.o rdx_pat_search.c
ar rcv librdxpatc.a rdx_pat_search.o
ar ts librdxpatc.a

# build dynamic library and set up links
rm -rf librdxpatc.so.1.0.1
gcc -shared -Wl,-soname,librdxpatc.so.1 -o librdxpatc.so.1.0.1 rdx_pat_search.o -lc
ln -sf librdxpatc.so.1.0.1 librdxpatc.so.1
ln -sf librdxpatc.so.1.0.1 librdxpatc.so

# compile test program and link with static library
#gcc -g -ansi -pedantic -Wall -static -o rdx_pat_test rdx_pat_test.c -L. -lrdxpatc -lm

# compile test program and link with shared library
gcc -g -ansi -pedantic -Wall -o rdx_pat_test rdx_pat_test.c -L. -lrdxpatc -lm

# run test program
./rdx_pat_test

# compile gbit() test routine
gcc -o test_gbit test_gbit.c -lm

# run gbit() test routine
./test_gbit

