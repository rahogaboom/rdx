
// Copyright (c) 1997-2016, Richard A Hogaboom
// All rights reserved.

/*
 * test cases and example code for the MKRdxPat class(in MKRdxPat.h)
 *
 * NOTES:
 *     1. all tests are independent.  any test may be deleted; it will affect no other test
 *     2. try the tests with different values for MAX_NUM_RDX_NODES, NUM_KEYS and NUM_KEY_BYTES
 *     3. each test outputs to it's own file of filename format: MKRdxPat.TESTn.results
 *     4. use the './MKRdxPat.mk' script to compile and execute the tests
 *     5. use './MKRdxPat.mk clean' to clean the directory
 */

/*
 * scrub sort() description and investigate sort()'ing node data
 * investigate if app_datap sort() return should be NULL if error condition
 * update References from Sedgewick books
 * read Google C++ Style Guide
 * do performance executable - do IPv4/IPv6 example
 */

#include <iostream>
#include <fstream>
#include <string>

#include "MKRdxPat.h"

using namespace MultiKeyRdxPat;

    void
print_key
    (
        unsigned char *key,
        ofstream& os,
        int NUM_KEYS,
        int NUM_KEY_BYTES
    )
{
    const size_t MSG_BUF_SIZE = 256;
    char string[MSG_BUF_SIZE];

    os << "kb key\n";
    for ( int k = 0 ; k < NUM_KEYS ; k++ )
    {
        for ( int b = 0 ; b < NUM_KEY_BYTES+1 ; b++ )
        {
            snprintf(string, MSG_BUF_SIZE, "%02x ", key[k*(NUM_KEY_BYTES+1) + b]);
            os << string;

        }

        switch ( key[k*(NUM_KEY_BYTES+1) + 0] )
        {
            case 0:
                os << "  // do not use key = " << k;
                break;

            case 1:
                os << "  // use key = " << k;
                break;

            default:
                os << "  // invalid key boolean(not 0 or 1) = " << k;
        }

        os << "\n";
    }
}

   int
main()
{
    {  // TEST 0
        int return_code;

        // application data of type app_data defined here
        struct app_data
        {
            int id;
        };

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 4;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        ofstream os;
        os.open("MKRdxPat.TEST0.results");

        os << "\n";
        os << "TEST 0: Print rdx trie before inserting any data nodes\n";
        os << "        Expected Results:\n";
        os << "           a. Only the impossible pre-allocated root node should be printed since\n";
        os << "              no other nodes have been inserted\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << "\n";
        os << "NUM_KEYS = " << NUM_KEYS << "\n";
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n";
        os << "rdx - Bytes allocated = " << rdx->size() << "\n\n";


        os << "a. Only the impossible pre-allocated root node should be printed since\n";
        os << "   no other nodes have been inserted\n\n";
        return_code = rdx->print(NULL, os);

        os << "return_code = rdx->print(NULL, os); return_code = " << return_code << "\n\n";

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    {  // TEST 1
        int return_code;

        // application data of type app_data defined here
        struct app_data
        {
            int id;
            double d;
        };

        app_data *app_datap;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 1;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0, sum = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1;  // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        ofstream os;
        os.open("MKRdxPat.TEST1.results");

        os << "\n";
        os << "TEST 1: Insert one data node in rdx trie, then insert node with same keys again\n";
        os << "        Expected Results:\n";
        os << "           a. One node insertion with return code 0\n";
        os << "           b. Total nodes allocated(not including root node) 1\n";
        os << "           c. No verification error\n";
        os << "           d. Print entire trie - allocated node and root node\n";
        os << "           e. Same keys node insertion should return code 1\n";
        os << "           f. Different keys node insertion into full rdx should return code 2\n";
        os << "           g. Same keys node insertion with a key boolean set to 0 should return code 3\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << "\n";
        os << "NUM_KEYS = " << NUM_KEYS << "\n";
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n";
        os << "rdx - Bytes allocated = " << rdx->size() << "\n\n";


        os << "a. One node insertion with return code 0\n";
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        return_code = rdx->insert((unsigned char *)rdx_key[0], &app_datap);

        os << "return_code = rdx->insert((unsigned char *)rdx_key[0], &app_datap); return_code = " << return_code << "\n\n";


        os << "b. Total nodes allocated(not including root node) 1\n";
        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        app_datap->id = 20;
        os << "set app_datap->id = 20\n";
        os << "get app_datap->id = " << app_datap->id << "\n";
        app_datap->d = 2.0;
        os << "set app_datap->d = 2.0\n";
        os << "get app_datap->d = " << app_datap->d << "\n\n";

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";


        os << "c. No verification error\n";
        return_code = rdx->verify(ERR_CODE_PRINT, os);

        os << "return_code = rdx->verify(ERR_CODE_PRINT, os); verify success(0) or fail(!0) -> return_code = " << return_code << "\n\n";


        os << "d. Print entire trie - allocated node and root node\n";
        return_code = rdx->print(NULL, os);

        os << "return_code = rdx->print(NULL, os); return_code = " << return_code << "\n\n";

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";


        os << "e. Same keys node insertion should return code 1\n";
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        return_code = rdx->insert((unsigned char *)rdx_key[0], &app_datap);

        os << "return_code = rdx->insert((unsigned char *)rdx_key[0], &app_datap); return_code = " << return_code << "\n\n";

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";


        os << "f. Different keys node insertion into full rdx should return code 2\n";
        // set each key to a unique value not used before - this will try to insert into a full rdx and set return_code = 2
        // - if any key is the same as a previously inserted node then return_code = 1 will be returned
        rdx_key[0][0][1] = 1;  // set first key(key index 0) first key byte to different from 0
        rdx_key[0][1][1] = 1;  // set second key(key index 1) first key byte to different from 0
        rdx_key[0][2][1] = 1;  // set third key(key index 2) first key byte to different from 0
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        return_code = rdx->insert((unsigned char *)rdx_key[0], &app_datap);

        os << "return_code = rdx->insert((unsigned char *)rdx_key[0], &app_datap); return_code = " << return_code << "\n\n";

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";


        os << "g. Same keys node insertion with a key boolean set to 0 should return code 3\n";
        rdx_key[0][1][0] = 0;  // set second key(key index 1) boolean to 0
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        return_code = rdx->insert((unsigned char *)rdx_key[0], &app_datap);

        os << "return_code = rdx->insert((unsigned char *)rdx_key[0], &app_datap); return_code = " << return_code << "\n\n";

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    {  // TEST 2
        int return_code;

        // application data of type app_data defined here
        struct app_data
        {
            int id;
            double d;
            double e[10];
        };

        app_data *app_datap;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 4;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0, sum = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1;  // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        ofstream os;
        os.open("MKRdxPat.TEST2.results");

        os << "\n";
        os << "TEST 2: Insert one data node - rdx->print(key, os) for rdx trie\n";
        os << "        Expected Results:\n";
        os << "           a. Should print only the data node with the passed in keys and all of the branch\n";
        os << "              nodes leading to that data node.  If there are N keys in that data node then\n";
        os << "              there will be N branch node sequences that lead to the same data node\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << "\n";
        os << "NUM_KEYS = " << NUM_KEYS << "\n";
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n";
        os << "rdx - Bytes allocated = " << rdx->size() << "\n\n";


        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        return_code = rdx->insert((unsigned char *)rdx_key[0], &app_datap);

        os << "return_code = rdx->insert((unsigned char *)rdx_key[0], &app_datap); return_code = " << return_code << "\n\n";


        os << "a. Should print only the data node with the passed in keys and all of the branch\n";
        os << "   nodes leading to that data node.  If there are N keys in that data node then\n";
        os << "   there will be N branch node sequences that lead to the same data node\n\n";
        return_code = rdx->print((unsigned char *)rdx_key[0], os);

        os << "return_code = rdx->print((unsigned char *)rdx_key[0], os); print success(0) or fail(!0) -> return code = " << return_code << "\n";

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    {  // TEST 3
        int return_code;

        // application data of type app_data defined here
        struct app_data
        {
            int id;
            double d;
            struct x
            {
                int j;
            };
        };

        app_data *app_datap;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 6;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0, sum = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1;  // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        ofstream os;
        os.open("MKRdxPat.TEST3.results");

        os << "\n";
        os << "TEST 3: Insert four data nodes each with NUM_KEYS keys in rdx trie\n";
        os << "        Expected Results:\n";
        os << "           a. Four data node insertions with return code 0\n";
        os << "           b. Total nodes allocated(not including root node) 4\n";
        os << "           c. No verification error\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << "\n";
        os << "NUM_KEYS = " << NUM_KEYS << "\n";
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n";
        os << "rdx - Bytes allocated = " << rdx->size() << "\n\n";


        os << "a. Four data node insertions with return code 0\n";
        for ( int n = 0, id = 10, d = 2.0 ; n < 4 ; n++, id++, d++ )
        {
            print_key((unsigned char *)rdx_key[n], os, NUM_KEYS, NUM_KEY_BYTES);
            return_code = rdx->insert((unsigned char *)rdx_key[n], &app_datap);

            os << "return_code = rdx->insert((unsigned char *)rdx_key[" << n << "], &app_datap); return_code = " << return_code << "\n\n";

            if ( return_code == 0 )
            {
                app_datap->id = id;
                os << "set app_datap->id = " << id << "\n";
                os << "get app_datap->id = " << app_datap->id << "\n";
                app_datap->d = d;
                os << "set app_datap->d = " << d << "\n";
                os << "get app_datap->d = " << app_datap->d << "\n\n";
            }
        }


        os << "b. Total nodes allocated(not including root node) 4\n";
        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";


        os << "c. No verification error\n\n";
        return_code = rdx->verify(ERR_CODE_PRINT, os);

        os << "return_code = rdx->verify(ERR_CODE_PRINT, os); verify success(0) or fail(!0) -> return_code = " << return_code << "\n\n";

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    {  // TEST 4
        int return_code;

        // application data of type app_data defined here
        struct app_data
        {
            int id;
            double d;
            struct x
            {
                int j;
                double d;
            };
        };

        app_data *app_datap;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 8;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0, sum = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1;  // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        ofstream os;
        os.open("MKRdxPat.TEST4.results");

        os << "\n";
        os << "TEST 4: Search for all MAX_NUM_RDX_NODES data nodes with NUM_KEYS keys in rdx trie\n";
        os << "        Expected Results:\n";
        os << "           a. insert 4 data nodes and search/find them\n";
        os << "           b. fail to find 4 nodes who's keys were not inserted\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << "\n";
        os << "NUM_KEYS = " << NUM_KEYS << "\n";
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n";
        os << "rdx - Bytes allocated = " << rdx->size() << "\n\n";


        os << "a. insert 4 data nodes and search/find them\n";
        for ( int n = 0 ; n < 4 ; n++ )
        {
            print_key((unsigned char *)rdx_key[n], os, NUM_KEYS, NUM_KEY_BYTES);
            return_code = rdx->insert((unsigned char *)rdx_key[n], &app_datap);

            os << "return_code = rdx->insert((unsigned char *)rdx_key[" << n << "], &app_datap); return_code = " << return_code << "\n\n";
        }

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";


        os << "b. fail to find 4 nodes who's keys were not inserted\n\n";
        for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            print_key((unsigned char *)rdx_key[n], os, NUM_KEYS, NUM_KEY_BYTES);
            app_datap = rdx->search((unsigned char *)rdx_key[n]);

            if (app_datap == NULL)
            {
                os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - search fail\n";
            }
            else
            {
                os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); search success\n";
            }
        }
        os << "\n";

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    {  // TEST 5
        int return_code;

        const size_t MSG_BUF_SIZE = 256;
        char string[MSG_BUF_SIZE];

        // application data of type app_data defined here
        struct app_data
        {
            int id;
            double d;
            struct x
            {
                int j;
                double d;
            };
            float f;
        };

        app_data *app_datap;

        app_data **app_datapp;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 4;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0, sum = MAX_NUM_RDX_NODES*NUM_KEYS ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1;  // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum--;
            }
        }

        ofstream os;
        os.open("MKRdxPat.TEST5.results");

        os << "\n";
        os << "TEST 5: Sort data nodes by successive keys in rdx trie\n";
        os << "        Expected Results:\n";
        os << "           a. Sort rdx with no nodes inserted and key index set to 3 - should return -1\n";
        os << "           b. Sort rdx with no nodes inserted and key index set to 0 - should return 0\n";
        os << "           c. Sort rdx with one node inserted and key index set to 0 - should return 1\n";
        os << "           d. For each key the return code will equal the number of sorted nodes and the\n";
        os << "              nodes array will hold the array of node pointers to nodes in sorted order\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << "\n";
        os << "NUM_KEYS = " << NUM_KEYS << "\n";
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n";
        os << "rdx - Bytes allocated = " << rdx->size() << "\n\n";


        os << "a. Sort rdx with no nodes inserted and key index set to 3 - should return -1\n";
        return_code = rdx->sort(&app_datapp, 3);

        os << "return_code = rdx->sort(&nodes, " << 3 << "); return_code = " << return_code << "\n\n";


        os << "b. Sort rdx with no nodes inserted and key index set to 0 - should return 0\n";
        return_code = rdx->sort(&app_datapp, 0);

        os << "return_code = rdx->sort(&nodes, " << 0 << "); return_code = " << return_code << "\n\n";


        os << "c. Sort rdx with one node inserted and key index set to 0 - should return 1\n";
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        return_code = rdx->insert((unsigned char *)rdx_key[0], &app_datap);

        os << "return_code = rdx->insert((unsigned char *)rdx_key[" << 0 << "], &app_datap); return_code = " << return_code << "\n\n";

        app_datap->id = 0;

        snprintf(string, MSG_BUF_SIZE, "n = %d  data.id = %08x\n\n", 0, app_datap->id);
        os << string;

        return_code = rdx->sort(&app_datapp, 0);

        os << "return_code = rdx->sort(&nodes, " << 0 << "); return_code = " << return_code << "\n\n";


        os << "d. For each key the return code will equal the number of sorted nodes and the\n";
        os << "   nodes array will hold the array of node pointers to nodes in sorted order\n";
        for ( int n = 1, sum = 1 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            print_key((unsigned char *)rdx_key[n], os, NUM_KEYS, NUM_KEY_BYTES);
            return_code = rdx->insert((unsigned char *)rdx_key[n], &app_datap);

            os << "return_code = rdx->insert((unsigned char *)rdx_key[" << n << "], &app_datap); return_code = " << return_code << "\n";

            app_datap->id = sum++;

            snprintf(string, MSG_BUF_SIZE, "n = %d  data.id = %08x\n\n", n, app_datap->id);
            os << string;
        }

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            os << "Sort the nodes by key " << k << ".\n\n";

            return_code = rdx->sort(&app_datapp, k);

            os << "return_code = rdx->sort(&nodes, " << k << "); return_code = " << return_code << "\n";

            for ( int n = 0 ; n < return_code ; n++ )
            {
                if ( app_datapp[n] == NULL )
                {
                    os << "n = " << n << " NULL\n";
                }
                else
                {
                    const size_t MSG_BUF_SIZE = 256;
                    char string[MSG_BUF_SIZE];

                    snprintf(string, MSG_BUF_SIZE, "n = %d  data.id = %08x\n", n, ((app_data *)app_datapp[n])->id);
                    os << string;
                }
            }

            os << "rdx - Total sorted nodes = " << return_code << "\n\n";
        }

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    {  // TEST 6
        int return_code;

        // application data of type app_data defined here
        struct app_data
        {
            int id;
            double d;
            struct x
            {
                int j;
                int k[5];
                double d;
            };
            float f;
        };

        app_data *app_datap;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 8;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0, sum = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1;  // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        ofstream os;
        os.open("MKRdxPat.TEST6.results");

        os << "\n";
        os << "TEST 6: Insert 5 data nodes in rdx trie - remove all nodes in rdx trie\n";
        os << "        Expected Results:\n";
        os << "           a. Non-NULL returns of the nodes removed(5), NULL returns(3) for nodes not\n";
        os << "              allocated and zero allocated nodes upon completion\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << "\n";
        os << "NUM_KEYS = " << NUM_KEYS << "\n";
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n";
        os << "rdx - Bytes allocated = " << rdx->size() << "\n\n";


        for ( int n = 0, sum = 0 ; n < 5 ; n++ )
        {
            const size_t MSG_BUF_SIZE = 256;
            char string[MSG_BUF_SIZE];

            print_key((unsigned char *)rdx_key[n], os, NUM_KEYS, NUM_KEY_BYTES);
            return_code = rdx->insert((unsigned char *)rdx_key[n], &app_datap);

            os << "return_code = rdx->insert((unsigned char *)rdx_key[" << n << "], &app_datap); return_code = " << return_code << "\n";

            app_datap->id = sum++;

            snprintf(string, MSG_BUF_SIZE, "n = %d  data.id = %08x\n\n", n, app_datap->id);
            os << string;
        }

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";


        os << "a. Non-NULL returns of the nodes removed(5), NULL returns(3) for nodes not\n";
        os << "   allocated and zero allocated nodes upon completion\n\n";
        for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            print_key((unsigned char *)rdx_key[n], os, NUM_KEYS, NUM_KEY_BYTES);
            app_datap = rdx->remove((unsigned char *)rdx_key[n]);
            if (app_datap == NULL)
            {
                os << "app_datap = rdx->remove((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - remove fail\n\n";
            }
            else
            {
                os << "app_datap = rdx->remove((unsigned char *)rdx_key[" << n << "]); remove successful\n\n";
            }
        }

        return_code = rdx->verify(ERR_CODE_PRINT, os);

        os << "return_code = rdx->verify(ERR_CODE_PRINT, os); verify success(0) or fail(!0) -> return_code = " << return_code << "\n\n";

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    {  // TEST 7
        int return_code;

        // application data of type app_data defined here
        struct app_data
        {
            int id;
            double d;
            struct x
            {
                int j;
                int k[5];
                double d;
            };
            float f;
            struct y
            {
                int j;
            };
        };

        app_data *app_datap;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 8;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));

        ofstream os;
        os.open("MKRdxPat.TEST7.results");

        os << "\n";
        os << "TEST 7: Insert/Search/Remove MAX_NUM_RDX_NODES nodes with random keys repeatedly\n";
        os << "        Expected Results:\n";
        os << "           a. Do not report success - report only errors in insert/search/remove operations\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << "\n";
        os << "NUM_KEYS = " << NUM_KEYS << "\n";
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n";
        os << "rdx - Bytes allocated = " << rdx->size() << "\n\n";


        os << "a. Do not report success - report only errors in insert/search/remove operations\n\n";
        int tot_errs;
        int test_num;

        for ( test_num = 0, tot_errs = 0 ; test_num < 32 ; test_num++ )
        {
            // in rdx_key[][][] generate MAX_NUM_RDX_NODES sets of NUM_KEYS keys each of NUM_KEY_BYTES in length
            srand(time(NULL));
            for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
            {
                for ( int k = 0 ; k < NUM_KEYS ; k++ )
                {
                    rdx_key[n][k][0] = 1;  // set key boolean to 1
                    for ( int b = 1 ; b < 1+NUM_KEY_BYTES ; b++ )
                    {
                        rdx_key[n][k][b] = rand() & 0x000000ff;
                    }
                }
            }

            // insert full set of MAX_NUM_RDX_NODES data nodes
            for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
            {
                return_code = rdx->insert((unsigned char *)rdx_key[n], &app_datap);

                os << "return_code = rdx->insert((unsigned char *)rdx_key[" << n << "], &app_datap); return_code = " << return_code << "\n";

                if ( return_code != 0 )
                {
                    tot_errs++;
                }
            }
            os << "\n";

            // search for full set of MAX_NUM_RDX_NODES data nodes
            for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
            {
                app_datap = rdx->search((unsigned char *)rdx_key[n]);

                if (app_datap == NULL)
                {
                    os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - search fail\n";
                    tot_errs++;
                }
                else
                {
                    os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); search success\n";
                }
            }
            os << "\n";

            // remove a full set of MAX_NUM_RDX_NODES data nodes
            for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
            {
                app_datap = rdx->remove((unsigned char *)rdx_key[n]);

                if (app_datap == NULL)
                {
                    os << "app_datap = rdx->remove((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - remove fail\n";
                    tot_errs++;
                }
                else
                {
                    os << "app_datap = rdx->remove((unsigned char *)rdx_key[" << n << "]); remove successful\n";
                }
            }

            os << "test = " << test_num << "  tot_errs = " << tot_errs << "\n";
            os << "\n\n";
        }
        os << " Total Insert/Search/Remove tests run " << test_num << "  Total errors detected " << tot_errs << "\n\n";

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    {  // TEST 8
        int return_code;

        int n = 0;

        // application data of type app_data defined here
        struct app_data
        {
            int id;
            double d;
            int n;
        };

        app_data *app_datap;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 1;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0, sum = 3 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1;  // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        ofstream os;
        os.open("MKRdxPat.TEST8.results");

        os << "\n";
        os << "TEST 8: Insert one data node in rdx with specific keys - key 0 = 3, key 1 = 4, key 2 = 5\n";
        os << "        Do, using only a single key, searches/removes with correct/incorrect keys\n";
        os << "        Expected Results:\n";
        os << "           a. insert one data node - should succeed\n";
        os << "           b. search on the inserted data node - should succeed\n";
        os << "           c. search using only one correct key - should succeed\n";
        os << "           d. search using only one incorrect key - should fail\n";
        os << "           e. search using a key boolean that is not 0 or 1 - should fail\n";
        os << "           f. search using using all key booleans 0 - should fail\n";
        os << "           g. print using only one key - should succeed\n";
        os << "           h. remove using only one incorrect key - should fail\n";
        os << "           i. remove using only one correct key - should succeed\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << "\n";
        os << "NUM_KEYS = " << NUM_KEYS << "\n";
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n";
        os << "rdx - Bytes allocated = " << rdx->size() << "\n\n";


        os << "a. insert one data node - should succeed\n";
        // key booleans:
        rdx_key[0][0][0] = 1;  // use first key 0 = 3
        rdx_key[0][1][0] = 1;  // use second key 1 = 4
        rdx_key[0][2][0] = 1;  // use third key 2 = 5
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        return_code = rdx->insert((unsigned char *)rdx_key[0], &app_datap);

        os << "return_code = rdx->insert((unsigned char *)rdx_key[" << n << "], &app_datap); return_code = " << return_code << "\n\n";

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";


        os << "b. search on the inserted data node - should succeed\n";
        // key booleans:
        rdx_key[0][0][0] = 1;  // use first key 0 = 3
        rdx_key[0][1][0] = 1;  // use second key 1 = 4
        rdx_key[0][2][0] = 1;  // use third key 2 = 5
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        app_datap = rdx->search((unsigned char *)rdx_key[0]);

        if (app_datap == NULL)
        {
            os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - search fail\n";
        }
        else
        {
            os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); search success\n";
        }
        os << "\n";


        os << "c. search using only one correct key - should succeed\n";
        // key booleans:
        rdx_key[0][0][0] = 0;  // ignore first key 0 = 3
        rdx_key[0][1][0] = 0;  // ignore second key 1 = 4
        rdx_key[0][2][0] = 1;  // use third key 2 = 5
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        app_datap = rdx->search((unsigned char *)rdx_key[0]);

        if (app_datap == NULL)
        {
            os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - search fail\n";
        }
        else
        {
            os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); search success\n";
        }
        os << "\n";


        os << "d. search using only one incorrect key - should fail\n";
        // key booleans:
        rdx_key[0][0][0] = 0;  // ignore first key 0 = 3
        rdx_key[0][1][0] = 0;  // ignore second key 1 = 4
        rdx_key[0][2][0] = 1;  // use third key 2 = 6 - wrong
        rdx_key[0][2][NUM_KEY_BYTES] = 6;  // use third key 2 = 6 - wrong
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        app_datap = rdx->search((unsigned char *)rdx_key[0]);

        if (app_datap == NULL)
        {
            os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - search fail\n";
        }
        else
        {
            os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); search success\n";
        }
        os << "\n";


        os << "e. search using a key boolean that is not 0 or 1 - should fail\n";
        // key booleans:
        rdx_key[0][0][0] = 0;  // ignore first key 0 = 3
        rdx_key[0][1][0] = 0;  // ignore second key 1 = 4
        rdx_key[0][2][0] = 2;  // use third key 2 = 5 - key boolean 2 is invalid
        rdx_key[0][2][NUM_KEY_BYTES] = 5;  // use third key 2 = 5
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        app_datap = rdx->search((unsigned char *)rdx_key[0]);

        if (app_datap == NULL)
        {
            os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - search fail\n";
        }
        else
        {
            os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); search success\n";
        }
        os << "\n";


        os << "f. search using using all key booleans 0 - should fail\n";
        // key booleans:
        rdx_key[0][0][0] = 0;  // ignore first key 0 = 3
        rdx_key[0][1][0] = 0;  // ignore second key 1 = 4
        rdx_key[0][2][0] = 0;  // use third key 2 = 5
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        app_datap = rdx->search((unsigned char *)rdx_key[0]);

        if (app_datap == NULL)
        {
            os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - search fail\n";
        }
        else
        {
            os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); search success\n";
        }
        os << "\n";


        os << "g. print using only one key - should succeed\n";
        // key booleans:
        rdx_key[0][0][0] = 0;  // ignore first key 0 = 3
        rdx_key[0][1][0] = 1;  // use second key 1 = 4
        rdx_key[0][2][0] = 0;  // ignore third key 2 = 5
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        return_code = rdx->print((unsigned char *)rdx_key[0], os);

        os << "return_code = rdx->print((unsigned char *)rdx_key[0], os); print success(0) or fail(!0) -> return code = " << return_code << "\n";
        os << "\n";


        os << "h. remove using only one incorrect key - should fail\n";
        // key booleans:
        rdx_key[0][0][0] = 0;  // ignore first key 0 = 3
        rdx_key[0][1][0] = 0;  // ignore second key 1 = 4
        rdx_key[0][2][0] = 1;  // use third key 2 = 6 - wrong
        rdx_key[0][2][NUM_KEY_BYTES] = 6;  // use third key 2 = 6 - wrong
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        app_datap = rdx->remove((unsigned char *)rdx_key[0]);

        if (app_datap == NULL)
        {
            os << "app_datap = rdx->remove((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - remove fail\n";
        }
        else
        {
            os << "app_datap = rdx->remove((unsigned char *)rdx_key[" << n << "]); remove successful\n";
        }
        os << "\n";


        os << "i. remove using only one correct key - should succeed\n";
        // key booleans:
        rdx_key[0][0][0] = 0;  // ignore first key 0 = 3
        rdx_key[0][1][0] = 0;  // ignore second key 1 = 4
        rdx_key[0][2][0] = 1;  // use third key 2 = 5
        rdx_key[0][2][NUM_KEY_BYTES] = 5;  // use third key 2 = 5
        print_key((unsigned char *)rdx_key[0], os, NUM_KEYS, NUM_KEY_BYTES);
        app_datap = rdx->remove((unsigned char *)rdx_key[0]);

        if (app_datap == NULL)
        {
            os << "app_datap = rdx->remove((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - remove fail\n";
        }
        else
        {
            os << "app_datap = rdx->remove((unsigned char *)rdx_key[" << n << "]); remove successful\n";
        }
        os << "\n";

        os.close();
    }

    {  // TEST 9
        int return_code;

        // application data of type app_data defined here
        struct app_data
        {
            int id;
            double d;
            float f;
        };

        app_data *app_datap;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 4;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // 1+MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES+1][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, (MAX_NUM_RDX_NODES+1) * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0, sum = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1;  // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        ofstream os;
        os.open("MKRdxPat.TEST9.results");

        os << "\n";
        os << "TEST 9: Insert MAX_NUM_RDX_NODES+1 data nodes in a rdx trie of only MAX_NUM_RDX_NODES nodes\n";
        os << "        Expected Results:\n";
        os << "           a. MAX_NUM_RDX_NODES node insertions with return code 0\n";
        os << "           b. 1 node insertion with return code 2(no free nodes)\n";
        os << "           c. Total nodes allocated(not including root node) MAX_NUM_RDX_NODES\n";
        os << "           d. No verification error\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << "\n";
        os << "NUM_KEYS = " << NUM_KEYS << "\n";
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n";
        os << "rdx - Bytes allocated = " << rdx->size() << "\n\n";


        os << "a. MAX_NUM_RDX_NODES node insertions with return code 0\n";
        os << "b. 1 node insertion with return code 2(no free nodes)\n";
        os << "c. Total nodes allocated(not including root node) MAX_NUM_RDX_NODES\n";
        for ( int n = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
        {
            print_key((unsigned char *)rdx_key[n], os, NUM_KEYS, NUM_KEY_BYTES);
            return_code = rdx->insert((unsigned char *)rdx_key[n], &app_datap);

            os << "return_code = rdx->insert((unsigned char *)rdx_key[" << n << "], &app_datap); return_code = " << return_code << "\n";

            os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";
        }
        os << "\n";


        os << "d. No verification error\n";
        return_code = rdx->verify(ERR_CODE_PRINT, os);

        os << "return_code = rdx->verify(ERR_CODE_PRINT, os); verify success(0) or fail(!0) -> return_code = " << return_code << "\n\n";

        os.close();
    }

    {  // TEST 10
        int return_code;
        int tot_errs = 0;

        // application data of type app_data defined here
        struct app_data
        {
            int id;
            double d;
            float f;
        };

        app_data *app_datap;
        app_data **app_datapp;


        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 255;

        // number of rdx search keys
        const int NUM_KEYS = 1;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 1;

        // 1+MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES+1][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, (MAX_NUM_RDX_NODES+1) * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0, sum = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1;  // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        ofstream os;
        os.open("MKRdxPat.TEST10.results");

        os << "\n";
        os << "TEST 10: Create rdx trie with 1 key of 1 byte and 255 data nodes\n";
        os << "         Expected Results:\n";
        os << "            a. MAX_NUM_RDX_NODES node insertions with return code 0\n";
        os << "            b. Total nodes allocated(not including root node) MAX_NUM_RDX_NODES\n";
        os << "            c. Search for all MAX_NUM_RDX_NODES data nodes - total errors = 0\n";
        os << "            d. Sort rdx - should return 255\n";
        os << "            e. Remove all data nodes - should return 0\n";
        os << "            f. Only the impossible pre-allocated root node should be printed since\n";
        os << "            g. No verification error\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << "\n";
        os << "NUM_KEYS = " << NUM_KEYS << "\n";
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n";
        os << "rdx - Bytes allocated = " << rdx->size() << "\n\n";


        os << "a. MAX_NUM_RDX_NODES node insertions with return code 0\n";
        for ( int n = 0, id = 10, d = 2.0 ; n < MAX_NUM_RDX_NODES ; n++, id++, d++ )
        {
            print_key((unsigned char *)rdx_key[n], os, NUM_KEYS, NUM_KEY_BYTES);
            return_code = rdx->insert((unsigned char *)rdx_key[n], &app_datap);

            os << "return_code = rdx->insert((unsigned char *)rdx_key[" << n << "], &app_datap); return_code = " << return_code << "\n\n";

            if ( return_code == 0 )
            {
                app_datap->id = id;
                os << "set app_datap->id = " << id << "\n";
                os << "get app_datap->id = " << app_datap->id << "\n";
                app_datap->d = d;
                os << "set app_datap->d = " << d << "\n";
                os << "get app_datap->d = " << app_datap->d << "\n\n";
            }
        }


        os << "b. Total nodes allocated(not including root node) MAX_NUM_RDX_NODES\n";
        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";


        os << "c. Search for all MAX_NUM_RDX_NODES data nodes - total errors = 0\n";
        for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            app_datap = rdx->search((unsigned char *)rdx_key[n]);

            if (app_datap == NULL)
            {
                os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - search fail\n";
                tot_errs++;
            }
            else
            {
                os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); search success\n";
            }
        }
        os << "\n";

        os << "Total errors detected " << tot_errs << "\n\n";


        os << "d. Sort rdx - should return 255\n";
        return_code = rdx->sort(&app_datapp, 0);

        os << "return_code = rdx->sort(&nodes, " << 0 << "); return_code = " << return_code << "\n\n";


        os << "e. Remove all data nodes - should return 0\n";
        for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            app_datap = rdx->remove((unsigned char *)rdx_key[n]);

            if (app_datap == NULL)
            {
                os << "app_datap = rdx->remove((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - remove fail\n";
                tot_errs++;
            }
            else
            {
                os << "app_datap = rdx->remove((unsigned char *)rdx_key[" << n << "]); remove successful\n";
            }
        }
        os << "\n";


        os << "f. Only the impossible pre-allocated root node should be printed since\n";
        os << "   all data nodes have been removed\n\n";
        return_code = rdx->print(NULL, os);

        os << "return_code = rdx->print(NULL, os); return_code = " << return_code << "\n\n";


        os << "g. No verification error\n";
        return_code = rdx->verify(ERR_CODE_PRINT, os);

        os << "return_code = rdx->verify(ERR_CODE_PRINT, os); verify success(0) or fail(!0) -> return_code = " << return_code << "\n\n";

        os.close();
    }

    {  // TEST 11
        int return_code;
        int tot_errs = 0;

        // application data of type app_data defined here
        struct app_data
        {
            int id;
            double d;
            float f;
        };

        app_data *app_datap;


        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 2;

        // number of rdx search keys
        const int NUM_KEYS = 8;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 16;

        // 1+MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES+1][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, (MAX_NUM_RDX_NODES+1) * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0, sum = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1;  // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        ofstream os;
        os.open("MKRdxPat.TEST11.results");

        os << "\n";
        os << "TEST 11: Create rdx trie with 8 keys of 16 bytes and 2 data nodes\n";
        os << "         Expected Results:\n";
        os << "            a. MAX_NUM_RDX_NODES node insertions with return code 0\n";
        os << "            b. Total nodes allocated(not including root node) MAX_NUM_RDX_NODES\n";
        os << "            c. Search for all MAX_NUM_RDX_NODES data nodes - total errors = 0\n";
        os << "            d. Print entire trie - allocated node and root node\n";
        os << "            e. No verification error\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << "\n";
        os << "NUM_KEYS = " << NUM_KEYS << "\n";
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n";
        os << "rdx - Bytes allocated = " << rdx->size() << "\n\n";


        os << "a. MAX_NUM_RDX_NODES node insertions with return code 0\n";
        for ( int n = 0, id = 10, d = 2.0 ; n < MAX_NUM_RDX_NODES ; n++, id++, d++ )
        {
            print_key((unsigned char *)rdx_key[n], os, NUM_KEYS, NUM_KEY_BYTES);
            return_code = rdx->insert((unsigned char *)rdx_key[n], &app_datap);

            os << "return_code = rdx->insert((unsigned char *)rdx_key[" << n << "], &app_datap); return_code = " << return_code << "\n\n";

            if ( return_code == 0 )
            {
                app_datap->id = id;
                os << "set app_datap->id = " << id << "\n";
                os << "get app_datap->id = " << app_datap->id << "\n";
                app_datap->d = d;
                os << "set app_datap->d = " << d << "\n";
                os << "get app_datap->d = " << app_datap->d << "\n\n";
            }
        }


        os << "b. Total nodes allocated(not including root node) MAX_NUM_RDX_NODES\n";
        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";


        os << "c. Search for all MAX_NUM_RDX_NODES data nodes - total errors = 0\n";
        for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            app_datap = rdx->search((unsigned char *)rdx_key[n]);

            if (app_datap == NULL)
            {
                os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); app_datap = NULL - search fail\n";
                tot_errs++;
            }
            else
            {
                os << "app_datap = rdx->search((unsigned char *)rdx_key[" << n << "]); search success\n";
            }
        }
        os << "\n";

        os << "Total errors detected " << tot_errs << "\n\n";


        os << "d. Print entire trie - allocated node and root node\n";
        return_code = rdx->print(NULL, os);

        os << "return_code = rdx->print(NULL, os); return_code = " << return_code << "\n\n";


        os << "e. No verification error\n";
        return_code = rdx->verify(ERR_CODE_PRINT, os);

        os << "return_code = rdx->verify(ERR_CODE_PRINT, os); verify success(0) or fail(!0) -> return_code = " << return_code << "\n\n";

        os.close();
    }

    {  // TEST 12
        int return_code;

        // application data of type app_data defined here
        struct app_data
        {
            int id;
            double d;
            float f;
        };

        app_data *app_datap;


        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 4;

        // number of rdx search keys
        const int NUM_KEYS = 4;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 2;

        // 1+MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES+1][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, (MAX_NUM_RDX_NODES+1) * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0, sum = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++, sum++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1;  // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum;
            }
        }

        ofstream os;
        os.open("MKRdxPat.TEST12.results");

        os << "\n";
        os << "TEST 12: Create rdx trie with 4 keys of 2 bytes and 4 data nodes with\n";
        os << "         key values the same for each key but unique within a key\n";
        os << "         Expected Results:\n";
        os << "            a. MAX_NUM_RDX_NODES node insertions with return code 0\n";
        os << "            b. Print entire trie - allocated node and root node\n";
        os << "            c. No verification error\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << "\n";
        os << "NUM_KEYS = " << NUM_KEYS << "\n";
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n";
        os << "rdx - Bytes allocated = " << rdx->size() << "\n\n";


        os << "a. MAX_NUM_RDX_NODES node insertions with return code 0\n";
        for ( int n = 0, id = 10, d = 2.0 ; n < MAX_NUM_RDX_NODES ; n++, id++, d++ )
        {
            print_key((unsigned char *)rdx_key[n], os, NUM_KEYS, NUM_KEY_BYTES);
            return_code = rdx->insert((unsigned char *)rdx_key[n], &app_datap);

            os << "return_code = rdx->insert((unsigned char *)rdx_key[" << n << "], &app_datap); return_code = " << return_code << "\n\n";
        }


        os << "b. Print entire trie - allocated node and root node\n";
        return_code = rdx->print(NULL, os);

        os << "return_code = rdx->print(NULL, os); return_code = " << return_code << "\n\n";


        os << "c. No verification error\n";
        return_code = rdx->verify(ERR_CODE_PRINT, os);

        os << "return_code = rdx->verify(ERR_CODE_PRINT, os); verify success(0) or fail(!0) -> return_code = " << return_code << "\n\n";

        os.close();
    }
}

