// performance measurements for rdx->insert()/rdx->remove()/rdx->search() library routines

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <locale.h>

#include "MKRdxPat.h"

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

/*
 * Notes:
 */

    int
main
    (
        int argc,
        char **argv
    )
{
    unsigned int rdx_size;

    /*
     * PNODE rdx1 related
     */
    MKRdxPat *rdx1 = new MKRdxPat( max_num_rdx_nodes, num_keys, num_key_bytes );

    /*
     * holds sets of random keys for MAX_NUM_RDX_NODES sets with num_keys keys of
     * num_key_bytes length with all key booleans set to 1
     */
    unsigned char rdx1_key[MAX_NUM_RDX_NODES][num_keys][1+num_key_bytes];

    DNODE *dnp; // data node pointer
    FILE *fp; // test output


    int pmode_opt = 1; // performance test case option
    double rtime_opt = 60.; // run time option
    int block_multiply_opt = 500; // block multiply option

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
                    fprintf(stderr, "-c option out of range(1 or 2): %d\n", pmode_opt);
                    exit(0);
                }
                break;

            case 's':
                rtime_opt = atof(optarg);
                if ( rtime_opt < 1 || pmode_opt > 86400 )
                {
                    fprintf(stderr, "-s option out of range(1 to 86400): %f\n", rtime_opt);
                    exit(0);
                }
                break;

            case 'b':
                block_multiply_opt = atoi(optarg);
                if ( block_multiply_opt < 1 || block_multiply_opt > 100000 )
                {
                    fprintf(stderr, "-b option out of range(1 to 100000): %d\n", block_multiply_opt);
                    exit(0);
                }
                break;

            case '?':
                fprintf(stderr, "./rd_pat_perf [-c] [-s]\n");
                return 1;

            default:
                fprintf(stderr, "abort(): %d\n", opt);
                abort();
        }
    }

    fp = fopen("rdx_pat_perf.results", "a");

    // initialize rdx1 PNODE structure
    rdx_size = rdx_pat_initialize(&rdx1);
    if ( rdx_size == 0 )
    {
        fprintf(fp, "rdx_pat_initialize(&rdx1); FAIL\n");
        exit(1);
    }

    /*
     * in rdx1_key[][][] generate MAX_NUM_RDX_NODES sets of num_keys random keys each of
     * num_key_bytes in length and set all key booleans to 1
     */
    srand(time(NULL));
    for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
    {
        for ( int k = 0 ; k < num_keys ; k++ )
        {
            rdx1_key[n][k][0] = 1; // set key boolean to 1
            for ( int b = 1 ; b < 1+num_key_bytes ; b++ )
            {
                rdx1_key[n][k][b] = rand() & 0x000000ff; // not crypto random - will produce some duplicates - ok
            }
        }
    }

    switch ( pmode_opt )
    {
        case 1:
            {
                long total_inserts_removes = 0;
                struct timespec tstart={0,0}, tend={0,0}, tdiff={0,0};
                double sec;


                fprintf(fp, "####################################################################################################\n");
                fprintf(fp, "PERFORMANCE TEST: Do repeated rdx->insert()(fill trie)/rdx->remove()(empty trie) - random keys\n");
                fprintf(fp, "                  MAX_NUM_RDX_NODES = %d  num_keys = %d  num_key_bytes = %d  trie size = %db\n",
                    max_num_rdx_nodes, num_keys, num_key_bytes, rdx_size);
                fprintf(fp, "                      Modify rdx_pat_search_perf.h with new parameters and re-compile.\n");
                fprintf(fp, "                  Minimum run time(sec): %f\n", rtime_opt);
                fprintf(fp, "                  Block Muliplier: %d\n", block_multiply_opt);
                fprintf(fp, "                  Insert/Delete increments: %d(%d*2*MAX_NUM_RDX_NODES)\n\n",
                    block_multiply_opt*2*max_num rdx_nodes, block_multiply_opt);

                clock_gettime(CLOCK_MONOTONIC, &tstart);

                for (;;)
                {
                    for ( int i = 0 ; i < block_multiply_opt ; i++ )
                    {
                        for ( int n = 0 ; n < max_num_rdx_nodes ; n++ )
                        {
                            rdx1->insert(rdx1_key[n], &dnp);
                        }

                        for ( int n = 0 ; n < max_num_rdx_nodes ; n++ )
                        {
                            rdx1->remove(rdx1_key[n]);
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
                fprintf(fp, "seconds = %f  total inserts/removes = %ld\n\n", sec, total_inserts_removes);
            }
            break;

        case 2:
            {
                long total_searches = 0;
                int random[MAX_NUM_RDX_NODES];
                struct timespec tstart={0,0}, tend={0,0}, tdiff={0,0};
                double sec;


                fprintf(fp, "####################################################################################################\n");
                fprintf(fp, "PERFORMANCE TEST: Do repeated rdx->search() - random keys\n");
                fprintf(fp, "                  MAX_NUM_RDX_NODES = %d  num_keys = %d  num_key_bytes = %d  trie size = %db\n",
                    max_num_rdx_nodes, num_keys, num_key_bytes, rdx_size);
                fprintf(fp, "                      Modify rdx_pat_search_perf.h with new parameters and re-compile.\n");
                fprintf(fp, "                  Minimum run time(sec): %f\n", rtime_opt);
                fprintf(fp, "                  Block Muliplier: %d\n", block_multiply_opt);
                fprintf(fp, "                  Search increments: %d(%d*MAX_NUM_RDX_NODES)\n\n",
                    block_multiply_opt*max_num_rdx_nodes, block_multiply_opt);

                srand(time(NULL));
                for ( int n = 0 ; n < max_num_rdx_nodes ; n++ )
                {
                    rdx->insert(&rdx1, rdx1_key[n], &dnp);
                    random[n] = rand() % max_num_rdx_nodes; // not crypto random - will produce some duplicates - ok
                }

                clock_gettime(CLOCK_MONOTONIC, &tstart);

                for (;;)
                {
                    for ( int i = 0 ; i < block_multiply_opt ; i++ )
                    {
                        for ( int n = 0 ; n < max_num_rdx_nodes ; n++ )
                        {
                            rdx->search(&rdx1, rdx1_key[random[n]]);
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
                fprintf(fp, "seconds = %f  total searches = %ld\n\n", sec, total_searches);
            }
            break;

        default:
            {
                fprintf(stderr, "Bad -c option.\n");
            }
            break;
    }
}

