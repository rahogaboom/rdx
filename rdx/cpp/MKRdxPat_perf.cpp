
/*
 * NAME
 *
 *     MKRdxPat_perf
 *
 * USAGE
 *
 *     ./MKRdxPat_perf [-c{1-2}] [-s{1-86400}] [-b{1-100000}]
 *
 * ARGUMENTS
 *
 *     None
 *
 * OPTIONS
 *
 *     -c{1-2}      - option 1: repeatedly insert()(fill)/remove()(empty) trie(default)
 *                    option 2: fill trie then do search()'s
 *
 *     -s{1-86400}  - minimum run time(secs)(30 default)
 *
 *     -b{1-100000} - c option 1: trie will be filled/emptied this many times
 *                    c option 2: every key of full trie will be searched for this many times
 *                    100 default
 *
 * DESCRIPTION
 *
 *     MKRdxPat_perf is used to evaluate the performance of the MKRdxPat class
 *     (Multi-Key Radix Fast Search) algorithm implementation.  first change
 *     the three arguments:
 *         const int max_num_rdx_nodes = 200000;
 *         const int num_keys          = 2;
 *         const int num_key_bytes     = 16;
 *     to match your application.  then insert in 'struct app_data{}' your application data
 *     node.  Re-compile.  run ./MKRdxPat_perf.  examine the MKRdxPat_perf.results file.
 *     see EXAMPLE OUTPUT below.  the file begins with a data/time and which -c option was
 *     run.  an lscpu cmd is run.  this provides some context on the hardware that was used
 *     to run the performance test.  the constructor arguments, the trie size, the option
 *     parameters used are given.  the incremental number of operations(insert(),remove(),
 *     serach()) in one iteration of the -b option is given.  finally, the time(secs) the
 *     run took and the number of operations performed in that time is output.  subsequent
 *     runs will append new data.
 *
 * NOTES
 *
 *     1. you need to select the three constructor arguments based on your application.
 *        remember that num_key_bytes is set to the longest key length.  see the MKRdxPat.hpp
 *        Description section for a detailed explanation of setting key arrays.
 *
 *     2. a more realistic performance evaluation my be had by modifying the code here to
 *        add additional steps always or typically used in your application.  for example,
 *        a call to memset() might be made before an insert()/remove()/search() since n
 *        many cases key arrays are filled starting with this.  also, some processing of
 *        node data might be done that is commonly application specific.
 *
 *     3. the default coded constructor arguments in the Description above were selected
 *        based on an application that accessed data nodes by IPv4 and IPv6 addresses, the
 *        two keys.  the longest is the IPv6 address at 16 bytes.  the number of data nodes
 *        was an arbitrary selection.  i've run tests at 2,000,000.  if, for example, the
 *        MAC address was added then the number of keys would be three, but with the MAC
 *        address length at 6 bytes, the IPv6 address would still be the longest.
 *
 * DEPENDENCIES
 *
 *     MKRdxPat.hpp class
 *
 * BUGS AND LIMITATIONS
 *
 *     No bugs known
 *
 * EXAMPLE OUTPUT
 *
 *     ####################################################################################################
 *     Wed Mar  2 16:28:19 2016
 *
 *     PERFORMANCE TEST: Do repeated rdx->insert()(fill trie) / rdx->remove()(empty trie)
 *
 *     lscpu:
 *     Architecture:          x86_64
 *     CPU op-mode(s):        32-bit, 64-bit
 *     Byte Order:            Little Endian
 *     CPU(s):                2
 *     On-line CPU(s) list:   0,1
 *     Thread(s) per core:    1
 *     Core(s) per socket:    2
 *     Socket(s):             1
 *     NUMA node(s):          1
 *     Vendor ID:             AuthenticAMD
 *     CPU family:            15
 *     Model:                 107
 *     Model name:            AMD Athlon(tm) 64 X2 Dual Core Processor 4000+
 *     Stepping:              1
 *     CPU MHz:               2109.639
 *     BogoMIPS:              4218.83
 *     Virtualization:        AMD-V
 *     L1d cache:             64K
 *     L1i cache:             64K
 *     L2 cache:              512K
 *     NUMA node0 CPU(s):     0,1
 *     Flags:                 fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ht syscall nx mmxext fxsr_opt rdtscp lm 3dnowext 3dnow rep_good nopl pni cx16 lahf_lm cmp_legacy svm extapic cr8_legacy 3dnowprefetch lbrv vmmcall
 *
 *     max_num_rdx_nodes = 200,000
 *     num_keys = 2
 *     num_key_bytes = 16
 *         (Modify MKRdxPat_perf.cpp with new parameters and re-compile.)
 *
 *     trie size = 42,000,242b
 *
 *     Minimum run time(sec): 30.000000
 *     Block Muliplier: 100
 *     insert()/remove() increments: 40,000,000(100*2*max_num_rdx_nodes)
 *
 *     seconds = 56.462147  total inserts/removes = 40,000,000
 *
 * SEE ALSO
 *
 *     MKRdxPat_test - the MKRdxPat.hpp class test suite
 *
 * AUTHOR
 *
 *     Richard A Hogaboom
 *     richard.hogaboom@gmail.com
 *
 * LICENSE and COPYRIGHT and (DISCLAIMER OF) WARRANTY
 *
 *     Copyright (c) 1998-2014, Richard A Hogaboom - richard.hogaboom@gmail.com
 *     All rights reserved.
 *
 *     Redistribution and use in source and binary forms, with or without
 *     modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice, this
 *       list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 * 
 *     * Neither the name of the Richard A Hogaboom nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 * 
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *     AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *     IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *     FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *     DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *     SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *     CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *     OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *     OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. :-)
 * 
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <ctime>

#include <unistd.h>

#include "MKRdxPat.hpp"

using namespace MultiKeyRdxPat;


// take the difference of two timespec structs and return this in a timespec struct
    struct timespec
timespec_diff
    (
        struct timespec start,
        struct timespec end
    )
{
    struct timespec temp;

    if ((end.tv_nsec - start.tv_nsec) < 0) 
    {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } 
    else 
    {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }

    return temp;
}

    int
main
    (
        int argc,
        char **argv
    )
{
    const int TMPSTR_SIZE = 256;
    char tmpstr[TMPSTR_SIZE];


    //
    // set these application specific data
    //
    // ================================================

    // MKRdxPat.hpp class constructor arguments
    const int max_num_rdx_nodes = 100000;
    const int num_keys          = 2;
    const int num_key_bytes     = 16;

    // application data of type app_data defined here
    struct app_data
    {
        int i;
    };

    // ================================================


    // holds sets of keys for max_num_rdx_nodes sets with num_keys keys of
    // num_key_bytes length with all key booleans set to 1
    // NOTE: the static keyword is needed to ensure that large rdx_key[][][]
    //       arrays do no blow the stack
    static unsigned char rdx_key[max_num_rdx_nodes][num_keys][1+num_key_bytes];

    app_data *app_datap;

    // enable printf("%'d", n); for commas in large numbers
    setlocale(LC_NUMERIC, "");

    ofstream os;
    os.open("MKRdxPat_perf.results", ofstream::app|ofstream::out);

    // cmd line option defaults
    int pmode_opt = 1;  // performance test case option
    double rtime_opt = 30.;  // run time option
    int block_multiply_opt = 100;  // block multiply option

    opterr = 0;
    int opt;
    while ( (opt = getopt(argc, argv, "c:s:b:")) != -1)
    {
        switch (opt)
        {
            case 'c':
                pmode_opt = atoi(optarg);
                if ( pmode_opt < 1 || pmode_opt > 2 )
                {
                    os << "-c option out of range(1 or 2): " << pmode_opt << "\n";
                    exit(0);
                }
                break;

            case 's':
                rtime_opt = atof(optarg);
                if ( rtime_opt < 1 || pmode_opt > 86400 )
                {
                    os << "-s option out of range(1 to 86400): " << rtime_opt << "\n";
                    exit(0);
                }
                break;

            case 'b':
                block_multiply_opt = atoi(optarg);
                if ( block_multiply_opt < 1 || block_multiply_opt > 100000 )
                {
                    os << "-b option out of range(1 to 100000): " << block_multiply_opt << "\n";
                    exit(0);
                }
                break;

            case '?':
                os << "./MKRdxPat_perf [-c{1-2}] [-s{1-86400}] [-b{1-100000}]\n";
                return 1;

            default:
                os << "abort(): " << opt << " = getopt(argc, argv, \"c:s:b:\")" << "\n";
                abort();
        }
    }

    // set gm time string
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = gmtime(&rawtime);
    strftime(tmpstr, TMPSTR_SIZE, "%c", timeinfo);

    os << "####################################################################################################\n";
    os << tmpstr << "\n\n";

    if ( pmode_opt == 1 )
    {
        os << "PERFORMANCE TEST: Do repeated rdx->insert()(fill trie) / rdx->remove()(empty trie)\n";
        os << "                  monatonic keys\n\nlscpu:\n\n";
    }
    if ( pmode_opt == 2 )
    {
        os << "PERFORMANCE TEST: Do repeated rdx->search()\n";
        os << "                  monatonic keys/random search\n\nlscpu:\n";
    }

    os.close();

    system("lscpu >> MKRdxPat_perf.results");

    os.open("MKRdxPat_perf.results", ofstream::app|ofstream::out);

    // rdx_key[][][] - generate max_num_rdx_nodes nodes of num_keys keys of
    // num_key_bytes bytes and set all key booleans to 1
    for ( int n = 0, sum = 0 ; n < max_num_rdx_nodes ; n++, sum++ )
    {
        for ( int k = 0 ; k < num_keys ; k++ )
        {
            rdx_key[n][k][0] = 1;  // set key boolean to 1
            memmove(&rdx_key[n][k][1], &sum, sizeof(sum));  // keys monatonically increase
        }
    }

    // MKRdxPat class trie constructor
    MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(max_num_rdx_nodes, num_keys, num_key_bytes);

    os << "\n";
    snprintf(tmpstr, sizeof(tmpstr), "\nmax_num_rdx_nodes = %'d\nnum_keys = %d\nnum_key_bytes = %d\n",
        max_num_rdx_nodes, num_keys, num_key_bytes);
    os << tmpstr;
    snprintf(tmpstr, sizeof(tmpstr), "    (Modify MKRdxPat_perf.cpp with new parameters and re-compile.)\n\n");
    os << tmpstr;
    snprintf(tmpstr, sizeof(tmpstr), "trie size = %'db\n\n", rdx->size());
    os << tmpstr;
    snprintf(tmpstr, sizeof(tmpstr), "Minimum run time(sec): %f\n", rtime_opt);
    os << tmpstr;
    snprintf(tmpstr, sizeof(tmpstr), "Block Muliplier: %d\n", block_multiply_opt);
    os << tmpstr;

    switch ( pmode_opt )
    {
        case 1:
            {
                long total_inserts_removes = 0;
                struct timespec tstart={0,0}, tend={0,0}, tdiff={0,0};
                double sec;
                int return_code;

                snprintf(tmpstr, sizeof(tmpstr), "insert()/remove() increments: %'d(%d*2*max_num_rdx_nodes)\n\n",
                    block_multiply_opt*2*max_num_rdx_nodes, block_multiply_opt);
                os << tmpstr;

                clock_gettime(CLOCK_MONOTONIC, &tstart);

                for (;;)
                {
                    for ( int i = 0 ; i < block_multiply_opt ; i++ )
                    {
                        for ( int n = 0 ; n < max_num_rdx_nodes ; n++ )
                        {
                            return_code = rdx->insert((unsigned char *)rdx_key[n], &app_datap);

                            if ( return_code != 0 )
                            {
                                os << "insert(): data node = " << n << " return_code = " << return_code << endl;
                            }
                        }

                        for ( int n = 0 ; n < max_num_rdx_nodes ; n++ )
                        {
                            app_datap = rdx->remove((unsigned char *)rdx_key[n]);

                            if ( app_datap == NULL )
                            {
                                os << "remove(): data node = " << n << " return = NULL" << endl;
                            }
                        }
                    }

                    total_inserts_removes += max_num_rdx_nodes*2*block_multiply_opt;

                    clock_gettime(CLOCK_MONOTONIC, &tend);

                    tdiff = timespec_diff(tstart, tend);
                    sec = tdiff.tv_sec + tdiff.tv_nsec/1E9;

                    if ( sec > rtime_opt )
                    {
                        break;
                    }
                }

                snprintf(tmpstr, sizeof(tmpstr), "seconds = %f  total inserts/removes = %'ld\n\n", sec, total_inserts_removes);
                os << tmpstr;
            }
            break;

        case 2:
            {
                long total_searches = 0;
                int random[max_num_rdx_nodes];  // for random key search()
                struct timespec tstart={0,0}, tend={0,0}, tdiff={0,0};
                double sec;

                snprintf(tmpstr, sizeof(tmpstr), "search() increments: %'d(%d*max_num_rdx_nodes)\n\n",
                    block_multiply_opt*max_num_rdx_nodes, block_multiply_opt);
                os << tmpstr;

                srand(time(NULL));
                for ( int n = 0 ; n < max_num_rdx_nodes ; n++ )
                {
                    rdx->insert((unsigned char *)rdx_key[n], &app_datap);
                    random[n] = rand() % max_num_rdx_nodes;  // not crypto random - will produce some duplicates - ok
                }

                clock_gettime(CLOCK_MONOTONIC, &tstart);

                for (;;)
                {
                    for ( int i = 0 ; i < block_multiply_opt ; i++ )
                    {
                        for ( int n = 0 ; n < max_num_rdx_nodes ; n++ )
                        {
                            app_datap = rdx->search((unsigned char *)rdx_key[random[n]]);

                            if ( app_datap == NULL )
                            {
                                os << "search(): data node = " << n << " return = NULL" << endl;
                            }

                            total_searches++;
                        }
                    }

                    clock_gettime(CLOCK_MONOTONIC, &tend);

                    tdiff = timespec_diff(tstart, tend);
                    sec = tdiff.tv_sec + tdiff.tv_nsec/1E9;

                    if ( sec > rtime_opt )
                    {
                        break;
                    }
                }

                snprintf(tmpstr, sizeof(tmpstr), "seconds = %f  total searches = %'ld\n\n", sec, total_searches);
                os << tmpstr;
            }
            break;

        default:
            {
                os << "Bad -c option.\n";
            }
            break;
    }
}

