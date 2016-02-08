#!/bin/bash

# C build script

set -v

if [ "$1" == 'clean' ]
then
    rm -f librdxpatc.a librdxpatc.so librdxpatc.so.1 librdxpatc.so.1.0.1
    rm -f rdx_pat_search1.o
    rm -f rdx_pat_test1 rdx_pat_test1.results rdx_pat_test2 rdx_pat_test2.results rdx_pat_test3 rdx_pat_test3.results
    rm -f test_gbit test_gbit.results
    rm -f rdx_pat_perf rdx_pat_perf.results
    exit
fi

# pick compiler - should work with all three
CC="clang"
#CC="gcc"
#CC="g++"

# build static library
rm -rf librdxpatc.a
$CC -std=c11 -c -fpic -g -pedantic -Wall -o rdx_pat_search1.o rdx_pat_search1.c
ar rcv librdxpatc.a rdx_pat_search1.o
ar ts librdxpatc.a

# build dynamic library and set up links
rm -rf librdxpatc.so.1.0.1
$CC -shared -Wl,-soname,librdxpatc.so.1 -o librdxpatc.so.1.0.1 rdx_pat_search1.o -lc
ln -sf librdxpatc.so.1.0.1 librdxpatc.so.1
ln -sf librdxpatc.so.1.0.1 librdxpatc.so

# compile test1 program and link with shared library
$CC -std=c11 -g -pedantic -Wall -o rdx_pat_test1 rdx_pat_test1.c -L. -lrdxpatc -lm

# compile test1 program
#clang -std=c11 -g -pedantic -Wall -o rdx_pat_test1 rdx_pat_test1.c rdx_pat_search1.c -lm
gcc -std=c11 -g -pedantic -Wall -o rdx_pat_test1 rdx_pat_test1.c rdx_pat_search1.c -lm
#g++ -std=c++11 -g -pedantic -Wall -o rdx_pat_test1 rdx_pat_test1.c rdx_pat_search1.c -lm

# run test1 program
./rdx_pat_test1

# compile test2 program
#clang -std=c11 -g -pedantic -Wall -o rdx_pat_test2 rdx_pat_test2.c rdx_pat_search2.c -lm
gcc -std=c11 -g -pedantic -Wall -o rdx_pat_test2 rdx_pat_test2.c rdx_pat_search2.c -lm
#g++ -std=c++11 -g -pedantic -Wall -o rdx_pat_test2 rdx_pat_test2.c rdx_pat_search2.c -lm

# run test2 program
./rdx_pat_test2

# compile test3 program
#clang -std=c11 -g -pedantic -Wall -o rdx_pat_test3 rdx_pat_test3.c rdx_pat_search3.c -lm
gcc -std=c11 -g -pedantic -Wall -o rdx_pat_test3 rdx_pat_test3.c rdx_pat_search3.c -lm
#g++ -std=c++11 -g -pedantic -Wall -o rdx_pat_test3 rdx_pat_test3.c rdx_pat_search3.c -lm

# run test3 program
./rdx_pat_test3

# compile perf program
#clang -std=gnu99 -g -pedantic -Wall -o rdx_pat_perf rdx_pat_search_perf.c rdx_pat_perf.c -lm
gcc -std=gnu99 -g -pedantic -Wall -o rdx_pat_perf rdx_pat_search_perf.c rdx_pat_perf.c -lm
#g++ -std=c++11 -g -pedantic -Wall -o rdx_pat_perf rdx_pat_search_perf.c rdx_pat_perf.c -lm

# compile gbit() test routine
#clang -std=c11 -o test_gbit test_gbit.c -lm
gcc -std=c11 -o test_gbit test_gbit.c -lm
#g++ -std=c++11 -o test_gbit test_gbit.c -lm

# run gbit() test routine
./test_gbit

