#!/bin/bash

# C++ build script

set -v

# add current directory to library path for shared libraries
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

# build static library
rm -rf librdxpatcpp.a
#g++ -c -fpic -g -ansi -pedantic -Wall -o rdx_pat_search.o rdx_pat_search.cpp
g++ -c -fpic -g -o rdx_pat_search.o rdx_pat_search.cpp
ar rcv librdxpatcpp.a rdx_pat_search.o
ar ts librdxpatcpp.a

# build dynamic library and set up links
rm -rf librdxpatcpp.so.1.0.1
g++ -shared -Wl,-soname,librdxpatcpp.so.1 -o librdxpatcpp.so.1.0.1 rdx_pat_search.o -lc
ln -sf librdxpatcpp.so.1.0.1 librdxpatcpp.so.1
ln -sf librdxpatcpp.so.1.0.1 librdxpatcpp.so

# compile test program and link with static library
#g++ -g -ansi -pedantic -Wall -static -o rdx_pat_test rdx_pat_test.cpp -L. -lrdxpatcpp -lm

# compile test program and link with shared library
g++ -g -ansi -pedantic -Wall -o rdx_pat_test rdx_pat_test.cpp -L. -lrdxpatcpp -lm

# run test program
./rdx_pat_test

# compile gbit() test routine
g++ -o test_gbit test_gbit.cpp -lm

# run gbit() test routine
./test_gbit

