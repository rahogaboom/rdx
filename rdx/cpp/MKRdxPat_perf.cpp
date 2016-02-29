
// Copyright (c) 1997-2016, Richard A Hogaboom
// All rights reserved.

// MKRdxPat.hpp class performance measurements for rdx->insert() / rdx->remove() / rdx->search() member functions

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

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
    const int MSG_BUF_SIZE = 256;
    char string[MSG_BUF_SIZE];

    unsigned int rdx_size = 0;

    const int max_num_rdx_nodes = 2000000;
    const int num_keys          = 2;
    const int num_key_bytes     = 16;

    // holds sets of random keys for max_num_rdx_nodes sets with num_keys keys of
    // num_key_bytes length with all key booleans set to 1
    // NOTE: the static keyword is needed to ensure that large rdx_key[][][]
    //       arrays do no blow the stack
    static unsigned char rdx_key[max_num_rdx_nodes][num_keys][1+num_key_bytes];

    // application data of type app_data defined here
    struct app_data
    {
        int i;
    };

    app_data *app_datap;

    ofstream os;
    os.open("MKRdxPat_perf.results", ofstream::app|ofstream::out);

    // cmd line option defaults
    int pmode_opt = 1;  // performance test case option
    double rtime_opt = 30.;  // run time option
    int block_multiply_opt = 10;  // block multiply option

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
                os << "./MKRdxPat_perf [-c] [-s]\n";
                return 1;

            default:
                os << "abort(): " << opt << "\n";
                abort();
        }
    }

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

    MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(max_num_rdx_nodes, num_keys, num_key_bytes);

    system("lscpu > cmd.lscpu");

    switch ( pmode_opt )
    {
        case 1:
            {
                long total_inserts_removes = 0;
                struct timespec tstart={0,0}, tend={0,0}, tdiff={0,0};
                double sec;
                int return_code;

                rdx_size = rdx->size();

                os << "####################################################################################################\n";
                os << "PERFORMANCE TEST: Do repeated rdx->insert()(fill trie) / rdx->remove()(empty trie) - random keys\n";
                snprintf(string, sizeof(string), "max_num_rdx_nodes = %d\nnum_keys = %d\nnum_key_bytes = %d\ntrie size = %db\n",
                    max_num_rdx_nodes, num_keys, num_key_bytes, rdx_size);
                os << string;
                snprintf(string, sizeof(string), "    (Modify MKRdxPat_perf.cpp with new parameters and re-compile.)\n");
                os << string;
                snprintf(string, sizeof(string), "Minimum run time(sec): %f\n", rtime_opt);
                os << string;
                snprintf(string, sizeof(string), "Block Muliplier: %d\n", block_multiply_opt);
                os << string;
                snprintf(string, sizeof(string), "insert()/remove() increments: %d(%d*2*max_num_rdx_nodes)\n\n",
                    block_multiply_opt*2*max_num_rdx_nodes, block_multiply_opt);
                os << string;

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

                snprintf(string, sizeof(string), "seconds = %f  total inserts/removes = %ld\n\n", sec, total_inserts_removes);
                os << string;
            }
            break;

        case 2:
            {
                long total_searches = 0;
                int random[max_num_rdx_nodes];
                struct timespec tstart={0,0}, tend={0,0}, tdiff={0,0};
                double sec;

                rdx_size = rdx->size();

                os << "####################################################################################################\n";
                os << "PERFORMANCE TEST: Do repeated rdx->search() - random keys\n";
                snprintf(string, sizeof(string), "max_num_rdx_nodes = %d\nnum_keys = %d\nnum_key_bytes = %d\ntrie size = %db\n",
                    max_num_rdx_nodes, num_keys, num_key_bytes, rdx_size);
                os << string;
                snprintf(string, sizeof(string), "    (Modify MKRdxPat_perf.cpp with new parameters and re-compile.)\n");
                os << string;
                snprintf(string, sizeof(string), "Minimum run time(sec): %f\n", rtime_opt);
                os << string;
                snprintf(string, sizeof(string), "Block Muliplier: %d\n", block_multiply_opt);
                os << string;
                snprintf(string, sizeof(string), "search() increments: %d(%d*max_num_rdx_nodes)\n\n",
                    block_multiply_opt*max_num_rdx_nodes, block_multiply_opt);
                os << string;

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
                            rdx->search((unsigned char *)rdx_key[random[n]]);
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

                snprintf(string, sizeof(string), "seconds = %f  total searches = %ld\n\n", sec, total_searches);
                os << string;
            }
            break;

        default:
            {
                os << "Bad -c option.\n";
            }
            break;
    }
}

