// test cases and example code for MKRdxPat.h class library routines

#include <iostream>
#include <fstream>

//#define DEBUG
#include "MKRdxPat.h"

using namespace MultiKeyRdxPat;
using namespace std;

#ifdef DEBUG

// for qsort() on a double field
    static int
double_compare
    (
        const void *a,
        const void *b
    )
{
    if ( (*(DNODE **)a)->data.data1.d < (*(DNODE **)b)->data.data1.d )
    {
        return 1;
    }
    else if ( (*(DNODE **)a)->data.data1.d > (*(DNODE **)b)->data.data1.d )
    {
        return -1;
    }

    return 0;
}

#endif


// print all NUM_KEYS keys in a data node
    void
print_keys
    (
        ofstream& fp,
        DNODE *dnp,
        int NUM_KEYS,
        int NUM_KEY_BYTES
    )
{
    if ( dnp == NULL )
    {
        fp << "Can't print data node keys: NULL data node\n";
    }
    else
    {
        char string[4];

        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            fp << "   key " << k << " = ";
            for ( int b = 0 ; b < NUM_KEY_BYTES ; b++ )
            {
                sprintf(string, "%02x ", dnp->key[k*(1+NUM_KEY_BYTES) + b+1]);
                fp << string;
            }
            fp << "\n";
        }
        fp << "\n";
    }
}


   int
main()
{
    { // TEST 0
        ofstream fp;
        fp.open("MKRdxPat.TEST0.results");

        fp << "\n";
        fp << "TEST 0: Print all allocated nodes plus the impossible key root node in rdx0 trie.\n";
        fp << "        Expected Results:\n";
        fp << "           a. Only the root node should be printed since no other nodes have been inserted\n\n";

        MKRdxPat *rdx0 = new MKRdxPat(8, 3, 4);

        fp << "TEST 0: rdx0 - Nodes allocated = " << rdx0->nodes() << "\n\n";

        rdx0->print(NULL, fp);

        fp << "TEST 0: rdx0 - Nodes allocated = " << rdx0->nodes() << "\n\n";

        fp.close();
    }

    { // TEST 1
        int return_code;

        ofstream fp;
        fp.open("MKRdxPat.TEST1.results");

        DNODE *dnp;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 8;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        /*
         * holds sets of random keys for MAX_NUM_RDX_NODES sets with NUM_KEYS keys of
         * NUM_KEY_BYTES length with all key booleans set to 1
         */
        unsigned char rdx1_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        /*
         * in rdx1_key[][][] generate MAX_NUM_RDX_NODES sets of NUM_KEYS random keys each of
         * NUM_KEY_BYTES in length and set all key booleans to 1
         */
        srand(time(NULL)); // comment out to get repeat sets of keys
        for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx1_key[n][k][0] = 1; // set key boolean to 1
                for ( int b = 1 ; b < 1+NUM_KEY_BYTES ; b++ )
                {
                    rdx1_key[n][k][b] = rand() & 0x000000ff;
                }
            }
        }

        fp << "\n";
        fp << "TEST 1: Insert one data node with NUM_KEYS random keys in rdx1 trie.\n";
        fp << "        Expected Results:\n";
        fp << "           a. One key insertion with return code 0\n";
        fp << "           b. Total nodes allocated(not including root node) 1\n";
        fp << "           c. No verification error\n";
        fp << "           d. Print entire trie - allocated node and root node\n\n";

        MKRdxPat *rdx1 = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        fp << "TEST 1: rdx1 - Nodes allocated = " << rdx1->nodes() << "\n\n";

        return_code = rdx1->insert((unsigned char *)rdx1_key[0], &dnp);

        fp << "TEST 1: rdx1->insert(rdx1_key[0], &dnp);  Return Code = " << return_code << "\n";

        fp << "TEST 1: the *dnp data node keys:\n";
        print_keys(fp, dnp, NUM_KEYS, NUM_KEY_BYTES);

        /*
         * set some data in the APP_DATA struct of the data node - see APP_DATA.h.
         * this is the user application data and is set after rdx_pat_insert() has returned
         * a pointer to a free data node with the keys now set to rdx1_key[n].  open
         * APP_DATA.h and install the APP_DATA structure that you want and use dnp to
         * set the structure data.  I only set id here.  for test I did not set the other APP_DATA
         * fields.  used id = 'aabbccdd' to be readily recognizable in the test output.
         */
        dnp->data.id = 0xaabbccdd;

        fp << "TEST 1: rdx1 - Nodes allocated = " << rdx1->nodes() << "\n\n";

        fp << "TEST 1: rdx1->verify(ERR_CODE_PRINT, fp); print diagnostics:\n\n";
        rdx1->verify(ERR_CODE_PRINT, fp);

        return_code = rdx1->verify(ERR_CODE, fp);
        if ( return_code != 0 )
        {
            fp << "TEST 1: rdx1->verify(ERR_CODE, fp); FAIL: verification error - " << return_code << "\n";
        }
        else
        {
            fp << "TEST 1: rdx1->verify(ERR_CODE, fp); verification successful\n";
        }

        fp << "TEST 1: rdx1->print(NULL, fp); print entire rdx1 trie:\n\n";
        rdx1->print(NULL, fp);

        fp << "TEST 1: rdx1 - Nodes allocated = " << rdx1->nodes() << "\n\n";

        fp.close();
    }


    { // TEST 2
        int return_code;

        ofstream fp;
        fp.open("MKRdxPat.TEST2.results");

        DNODE *dnp;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 8;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        /*
         * holds sets of random keys for MAX_NUM_RDX_NODES sets with NUM_KEYS keys of
         * NUM_KEY_BYTES length with all key booleans set to 1
         */
        unsigned char rdx2_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        /*
         * in rdx2_key[][][] generate MAX_NUM_RDX_NODES sets of NUM_KEYS random keys each of
         * NUM_KEY_BYTES in length and set all key booleans to 1
         */
        srand(time(NULL)); // comment out to get repeat sets of keys
        for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx2_key[n][k][0] = 1; // set key boolean to 1
                for ( int b = 1 ; b < 1+NUM_KEY_BYTES ; b++ )
                {
                    rdx2_key[n][k][b] = rand() & 0x000000ff;
                }
            }
        }

        fp << "\n";
        fp << "TEST 2: Test rdx2->print(key, fp) for rdx2 trie - insert one data node first.\n";
        fp << "        Expected Results:\n";
        fp << "           a. Should print only the data node with the passed in keys and all of the branch\n";
        fp << "              nodes leading to that data node.  If there are N keys in that data node then\n";
        fp << "              there will be N branch node sequences that lead to the same data node\n\n";

        MKRdxPat *rdx2 = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        fp << "TEST 2: rdx2 - Nodes allocated = " << rdx2->nodes() << "\n\n";

        return_code = rdx2->insert((unsigned char *)rdx2_key[0], &dnp);

        // rdx2->print(NULL, fp);
        rdx2->print((unsigned char *)rdx2_key[0], fp);

        fp << "TEST 2: rdx2 - Nodes allocated = " << rdx2->nodes() << "\n\n";

        fp.close();
    }

    { // TEST 3
        int return_code;

        ofstream fp;
        fp.open("MKRdxPat.TEST3.results");

        DNODE *dnp;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 8;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        /*
         * holds sets of random keys for MAX_NUM_RDX_NODES sets with NUM_KEYS keys of
         * NUM_KEY_BYTES length with all key booleans set to 1
         */
        unsigned char rdx3_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        /*
         * in rdx3_key[][][] generate MAX_NUM_RDX_NODES sets of NUM_KEYS random keys each of
         * NUM_KEY_BYTES in length and set all key booleans to 1
         */
        srand(time(NULL)); // comment out to get repeat sets of keys
        for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx3_key[n][k][0] = 1; // set key boolean to 1
                for ( int b = 1 ; b < 1+NUM_KEY_BYTES ; b++ )
                {
                    rdx3_key[n][k][b] = rand() & 0x000000ff;
                }
            }
        }

        fp << "\n";
        fp << "TEST 3: Insert four data nodes each with NUM_KEYS random keys in rdx3 trie.\n";
        fp << "        Expected Results:\n";
        fp << "           a. Four data node insertions with return code 0\n";
        fp << "           b. Total nodes allocated(not including root node) 4\n";
        fp << "           c. No verification error\n\n";

        MKRdxPat *rdx3 = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        fp << "TEST 3: rdx3 - Nodes allocated = " << rdx3->nodes() << "\n\n";

        for ( int n = 1 ; n < 5 ; n++ )
        {
            return_code = rdx3->insert((unsigned char *)rdx3_key[n], &dnp);

            fp << "TEST 3: rdx3->insert((unsigned char *)rdx3_key[" << n << "], &dnp); Return Code = " << return_code << "\n";

            fp << "TEST 3: the *dnp data node keys:\n";
            print_keys(fp, dnp, NUM_KEYS, NUM_KEY_BYTES);

            if ( return_code == 0 )
            {
                /*
                 * set some data in the APP_DATA struct of the data node - see APP_DATA.h.
                 * used 'aabbccdd' to be readily recognizable in the test output.
                 */
                dnp->data.id = 0xaabbccdd;
            }
        }
        fp << "TEST 3: rdx3 - Nodes allocated = " << rdx3->nodes() << "\n\n";

        fp << "TEST 3: call rdx3->verify(ERR_CODE_PRINT, fp); print diagnostics:\n\n";
        return_code = rdx3->verify(ERR_CODE_PRINT, fp);
        if ( return_code != 0 )
        {
            fp << "TEST 3: rdx3->verify(ERR_CODE_PRINT, fp); FAIL: verification error - " << return_code << "\n";
        }
        else
        {
            fp << "TEST 3: rdx3->verify(ERR_CODE_PRINT, fp); verification successful\n";
        }

        fp << "TEST 3: rdx3 - Nodes allocated = " << rdx3->nodes() << "\n\n";

        fp.close();
    }

    { // TEST 4
        int return_code;

        ofstream fp;
        fp.open("MKRdxPat.TEST4.results");

        DNODE *dnp;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 8;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        /*
         * holds sets of random keys for MAX_NUM_RDX_NODES sets with NUM_KEYS keys of
         * NUM_KEY_BYTES length with all key booleans set to 1
         */
        unsigned char rdx4_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        /*
         * in rdx4_key[][][] generate MAX_NUM_RDX_NODES sets of NUM_KEYS random keys each of
         * NUM_KEY_BYTES in length and set all key booleans to 1
         */
        srand(time(NULL)); // comment out to get repeat sets of keys
        for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx4_key[n][k][0] = 1; // set key boolean to 1
                for ( int b = 1 ; b < 1+NUM_KEY_BYTES ; b++ )
                {
                    rdx4_key[n][k][b] = rand() & 0x000000ff;
                }
            }
        }

        fp << "\n";
        fp << "TEST 4: Search for all MAX_NUM_RDX_NODES data nodes with NUM_KEYS random keys in rdx4 trie.\n";
        fp << "        Expected Results:\n";
        fp << "           a. find all 4 data nodes previously inserted\n";
        fp << "           b. fail to find 4 nodes who's keys were not inserted\n\n";

        MKRdxPat *rdx4 = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        fp << "TEST 4: rdx4 - Nodes allocated = " << rdx4->nodes() << "\n\n";

        for ( int n = 2 ; n < 6 ; n++ )
        {
            return_code = rdx4->insert((unsigned char *)rdx4_key[n], &dnp);

            fp << "TEST 4: rdx4->insert((unsigned char *)rdx4_key[" << n << "], &dnp); Return Code = " << return_code << "\n";

            fp << "TEST 4: the *dnp data node keys:\n";
            print_keys(fp, dnp, NUM_KEYS, NUM_KEY_BYTES);
        }

        fp << "TEST 4: rdx4 - Nodes allocated = " << rdx4->nodes() << "\n\n";

        for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            dnp = rdx4->search((unsigned char *)rdx4_key[n]);
            if (dnp == NULL)
            {
                fp << "TEST 4: rdx4->search(rdx4_key[" << n << "]); NULL return - search fail\n";
            }
            else
            {
                fp << "TEST 4: rdx4->search(rdx4_key[" << n << "]);\n";

                fp << "TEST 4: the *dnp data node keys:\n";
                print_keys(fp, dnp, NUM_KEYS, NUM_KEY_BYTES);
            }
        }

        fp << "TEST 4: rdx4 - Nodes allocated = " << rdx4->nodes() << "\n\n";

        fp.close();
    }

#ifdef DEBUG
    {
        fp << "\n");
        fp << "TEST 5: Sort data nodes by successive keys in rdx1 trie\n");
        fp << "        Expected Results:\n");
        fp << "           a. For each key the return code will equal the number of sorted nodes and the\n");
        fp << "              nodes array will hold the array of node pointers to nodes in sorted order\n\n");

        fp << "TEST 5: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            fp << "TEST 5: Sort the nodes by key %d.\n\n", k);
            return_code = rdx_pat_sort(&rdx1, &sorted_nodes, k);
            fp << "TEST 5: rdx_pat_sort(&rdx1, &sorted_nodes, %d); - Return code = %d\n", k, return_code);

            for ( int n = 0 ; n < return_code ; n++ )
            {
                if ( sorted_nodes[n] == NULL )
                {
                    fp << "TEST 5: n = %d NULL\n", n);
                }
                else
                {
                    fp << "TEST 5: n = %d  data.id = %08x\n\n", n, ((DNODE *)sorted_nodes[n])->data.id);

                    fp << "TEST 5: the *dnp data node keys:\n\n");
                    print_keys(fp, (DNODE *)sorted_nodes[n]);
                }
            }
            fp << "TEST 5: rdx1 - Total sorted keys = %d\n\n", return_code);
        }

        fp << "TEST 5: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

        fp.close();
    }

    {
        fp << "\n");
        fp << "TEST 6: Test to show how to sort nodes based on an arbitrary sort key in the APP_DATA struct\n");
        fp << "        Expected Results:\n");
        fp << "           a. Using data.data1.d double to sort on.  Nodes should be ordered by this field\n\n");

        fp << "TEST 6: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

        int tot_nodes;

        tot_nodes = rdx_pat_nodes(&rdx1);
        fp << "TEST 6: tot_nodes = %d\n", tot_nodes);

        for ( int n = 0 ; n < tot_nodes ; n++ )
        {
            ((DNODE *)rdx1.node_ptrs[n])->data.data1.d = n;
            fp << "TEST 6: node: %d  (void *)(DNODE *)rdx1.node_ptrs[%d]: %p  ((DNODE *)rdx1.node_ptrs[%d])->data.data1.d: %f\n",
                n, n, (void *)(DNODE *)rdx1.node_ptrs[n], n, ((DNODE *)rdx1.node_ptrs[n])->data.data1.d);
        }
        fp << "\n");

        qsort(&rdx1.node_ptrs[0], tot_nodes, sizeof(DNODE *), double_compare);

        for ( int n = 0 ; n < tot_nodes ; n++ )
        {
            fp << "TEST 6: node: %d  (void *)(DNODE *)rdx1.node_ptrs[%d]: %p  ((DNODE *)rdx1.node_ptrs[%d])->data.data1.d: %f\n",
                n, n, (void *)(DNODE *)rdx1.node_ptrs[n], n, ((DNODE *)rdx1.node_ptrs[n])->data.data1.d);
        }
        fp << "\n");

        fp << "TEST 6: rdx_pat_verify(&rdx1, ERR_CODE_PRINT, fp); with ERR_CODE_PRINT - print diagnostics:\n\n");
        return_code = rdx_pat_verify(&rdx1, ERR_CODE_PRINT, fp);
        if ( return_code != 0 )
        {
            fp << "TEST 6: rdx_pat_verify(&rdx1, ERR_CODE_PRINT, fp); FAIL: verification error - %d\n", return_code);
        }
        else
        {
            fp << "TEST 6: rdx_pat_verify(&rdx1, ERR_CODE_PRINT, fp); verification successful\n");
        }

        fp << "TEST 6: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

        fp.close();
    }

    {
        fp << "\n");
        fp << "TEST 7: Delete all keys in rdx1 trie\n");
        fp << "        Expected Results:\n");
        fp << "           a. Non-NULL returns of the nodes removed(5), NULL returns(3) for nodes not\n");
        fp << "              allocated and zero allocated nodes upon completion\n\n");

        fp << "TEST 7: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

        for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            fp << "TEST 7: rdx_pat_remove(&rdx1, rdx1_key[%d]);\n", n);
            dnp = rdx_pat_remove(&rdx1, rdx1_key[n]);
            if (dnp == NULL)
            {
                fp << "TEST 7: rdx_pat_remove(&rdx1, rdx1_key[%d]); - NULL return - remove fail\n", n);
            }
            else
            {
                fp << "TEST 7:  rdx_pat_remove(&rdx1, rdx1_key[%d]); - remove successful\n\n", n);

                fp << "TEST 7: the *dnp data node keys:\n\n");
                print_keys(fp, dnp);
            }
        }
        fp << "\n");

        fp << "TEST 7: rdx_pat_verify(&rdx1, ERR_CODE_PRINT, fp); with ERR_CODE_PRINT - print diagnostics:\n\n");
        return_code = rdx_pat_verify(&rdx1, ERR_CODE_PRINT, fp);
        if ( return_code != 0 )
        {
            fp << "TEST 7: rdx_pat_verify(&rdx1, ERR_CODE_PRINT, fp); FAIL: verification error - %d\n", return_code);
        }
        else
        {
            fp << "TEST 7: rdx_pat_verify(&rdx1, ERR_CODE_PRINT, fp); verification successful\n");
        }

        fp << "TEST 7: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

        fp.close();
    }

    {
        fp << "\n");
        fp << "TEST 8: Insert/Search/Delete MAX_NUM_RDX_NODES nodes with random keys repeatedly.\n");
        fp << "        Expected Results:\n");
        fp << "           a. Do not report success - report only errors; insert/search/remove operation failures\n\n");

        fp << "TEST 8: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

        int tot_errs;
        int test_num;

        for ( test_num = 0,tot_errs = 0 ; test_num < 32 ; test_num++ )
        {
            // in rdx1_key[][][] generate MAX_NUM_RDX_NODES sets of NUM_KEYS random keys each of NUM_KEY_BYTES in length
            srand(time(NULL));
            for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
            {
                for ( int k = 0 ; k < NUM_KEYS ; k++ )
                {
                    rdx1_key[n][k][0] = 1; // set key boolean to 1
                    for ( int b = 1 ; b < 1+NUM_KEY_BYTES ; b++ )
                    {
                        rdx1_key[n][k][b] = rand() & 0x000000ff;
                    }
                }
            }

            // insert full set of MAX_NUM_RDX_NODES data nodes
            for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
            {
                fp << "TEST 8: rdx_pat_insert(&rdx1, rdx1_key[%d], &dnp);\n", n);
                return_code = rdx_pat_insert(&rdx1, rdx1_key[n], &dnp);
                if ( return_code != 0 )
                {
                    fp << "TEST 8: rdx_pat_insert(&rdx1, rdx1_key[%d], &dnp); FAIL: error - %d\n", n, return_code);
                    tot_errs++;
                }
            }

            // search for full set of MAX_NUM_RDX_NODES data nodes
            for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
            {
                fp << "TEST 8: rdx_pat_search(&rdx1, rdx1_key[%d]);\n", n);
                dnp = rdx_pat_search(&rdx1, rdx1_key[n]);
                if (dnp == NULL)
                {
                    fp << "TEST 8: rdx_pat_search(&rdx1, rdx1_key[%d]); FAIL: error\n", n);
                    tot_errs++;
                }
            }

            // remove a full set of MAX_NUM_RDX_NODES data nodes
            for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
            {
                fp << "TEST 8: rdx_pat_remove(&rdx1, rdx1_key[%d]);\n", n);
                dnp = rdx_pat_remove(&rdx1, rdx1_key[n]);
                if (dnp == NULL)
                {
                    fp << "TEST 8: rdx_pat_remove(&rdx1, rdx1_key[%d]); FAIL: error\n", n);
                    tot_errs++;
                }
            }

            fp << "TEST 8: test = %d  tot_errs = %d\n", test_num, tot_errs);
            fflush(fp);
        }
        fp << "TEST 8 - Total Insert/Search/Delete tests run %d  Total errors detected %d\n", test_num, tot_errs);

        fp << "TEST 8: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

        fp.close();
    }

    {
        if ( NUM_KEYS != 3 || MAX_NUM_RDX_NODES < 2 )
        {
            fp << "####################################################################################################\n");
            fp << "TEST 9: Not done - needs NUM_KEYS = 3 and MAX_NUM_RDX_NODES >= 2.\n");
        }
        else
        {
            fp << "\n");
            fp << "TEST 9: Insert two data nodes in rdx1 with specific keys(keys 1,2,3 in data node 0 and keys 4,5,6 in data node 1).\n");
            fp << "        Verify inserts worked - verify searches/removes with mixed up keys fail.\n");
            fp << "        Expected Results:\n");
            fp << "           a. search on the two inserted data nodes should not report errors\n");
            fp << "           b. search on mixed up keys from both data nodes should fail\n");
            fp << "           c. remove on mixed up keys from both data nodes should fail\n\n");

            fp << "TEST 9: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                for ( int b = 1 ; b < 1+NUM_KEY_BYTES ; b++ )
                {
                    rdx1_key[0][k][b] = 0;
                    rdx1_key[1][k][b] = 0;
                }
            }

            rdx1_key[0][0][NUM_KEY_BYTES] = 1;
            rdx1_key[0][1][NUM_KEY_BYTES] = 2;
            rdx1_key[0][2][NUM_KEY_BYTES] = 3;

            rdx1_key[0][0][0] = 1; // use first key(0)
            rdx1_key[0][1][0] = 1; // use second key(1)
            rdx1_key[0][2][0] = 1; // use third key(2)

            return_code = rdx_pat_insert(&rdx1, rdx1_key[0], &dnp);
            fp << "TEST 9: rdx_pat_insert(&rdx1, rdx1_key[%d], &dnp); Return Code = %d\n\n", 0, return_code);

            fp << "TEST 9: the *dnp data node keys:\n\n");
            print_keys(fp, dnp);

            rdx1_key[1][0][NUM_KEY_BYTES] = 4;
            rdx1_key[1][1][NUM_KEY_BYTES] = 5;
            rdx1_key[1][2][NUM_KEY_BYTES] = 6;

            rdx1_key[1][0][0] = 1; // use first key(0)
            rdx1_key[1][1][0] = 1; // use second key(1)
            rdx1_key[1][2][0] = 1; // use third key(2)

            return_code = rdx_pat_insert(&rdx1, rdx1_key[1], &dnp);
            fp << "TEST 9: rdx_pat_insert(&rdx1, rdx1_key[%d], &dnp); Return Code = %d\n\n", 1, return_code);

            fp << "TEST 9: the *dnp data node keys:\n\n");
            print_keys(fp, dnp);

            dnp = rdx_pat_search(&rdx1, rdx1_key[0]);
            if (dnp == NULL)
            {
                fp << "TEST 9: rdx_pat_search(&rdx1, rdx1_key[0]); FAIL: first search fail - unexpected.\n");
            }
            else
            {
                fp << "TEST 9: rdx_pat_search(&rdx1, rdx1_key[0]); - first search succeeds - expected.\n");
            }

            dnp = rdx_pat_search(&rdx1, rdx1_key[1]);
            if (dnp == NULL)
            {
                fp << "TEST 9: rdx_pat_search(&rdx1, rdx1_key[1]); FAIL: second search fail - unexpected.\n");
            }
            else
            {
                fp << "TEST 9: rdx_pat_search(&rdx1, rdx1_key[1]); - second search succeeds - expected.\n");
            }
            fp << "\n");

            rdx1_key[0][0][NUM_KEY_BYTES] = 1;
            rdx1_key[0][1][NUM_KEY_BYTES] = 2;
            rdx1_key[0][2][NUM_KEY_BYTES] = 6;

            rdx1_key[0][0][0] = 1; // use first key(0)
            rdx1_key[0][1][0] = 1; // use second key(1)
            rdx1_key[0][2][0] = 1; // use third key(2)

            fp << "TEST 9: rdx_pat_search(&rdx1, rdx1_key[0]); - search for data node with keys 1,2,6.\n");
            dnp = rdx_pat_search(&rdx1, rdx1_key[0]);
            if (dnp == NULL)
            {
                fp << "TEST 9: rdx_pat_search(&rdx1, rdx1_key[0]); - search fails - keys in different data nodes - expected.\n");
            }
            fp << "\n");

            fp << "TEST 9: rdx_pat_remove(&rdx1, rdx1_key[0]); - remove data node with keys 1,2,6.\n");
            dnp = rdx_pat_remove(&rdx1, rdx1_key[0]);
            if (dnp == NULL)
            {
                fp << "TEST 9: rdx_pat_remove(&rdx1, rdx1_key[0]); - remove fails - keys in different data nodes - expected.\n");
            }
            fp << "\n");

            rdx1_key[1][0][NUM_KEY_BYTES] = 4;
            rdx1_key[1][1][NUM_KEY_BYTES] = 5;
            rdx1_key[1][2][NUM_KEY_BYTES] = 3;

            rdx1_key[1][0][0] = 1; // use first key(0)
            rdx1_key[1][1][0] = 1; // use second key(1)
            rdx1_key[1][2][0] = 1; // use third key(2)

            fp << "TEST 9: rdx_pat_search(&rdx1, rdx1_key[1]); - search for data node with keys 4,5,3.\n");
            dnp = rdx_pat_search(&rdx1, rdx1_key[1]);
            if (dnp == NULL)
            {
                fp << "TEST 9: rdx_pat_search(&rdx1, rdx1_key[1]); - search fails - keys in different data nodes - expected.\n");
            }
            fp << "\n");

            fp << "TEST 9: rdx_pat_remove(&rdx1, rdx1_key[1]); - remove data node with keys 4,5,3.\n");
            dnp = rdx_pat_remove(&rdx1, rdx1_key[1]);
            if (dnp == NULL)
            {
                fp << "TEST 9: rdx_pat_remove(&rdx1, rdx1_key[1]); - remove fails - keys in different data nodes - expected.\n");
            }

            fp << "TEST 9: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
        }

        fp.close();
    }

    {
        if ( NUM_KEYS != 3 || MAX_NUM_RDX_NODES < 2 )
        {
            fp << "####################################################################################################\n");
            fp << "TEST 10: Not done - needs NUM_KEYS = 3 and MAX_NUM_RDX_NODES >= 2.\n");
        }
        else
        {
            fp << "\n");
            fp << "TEST 10: Search for one data node in rdx1 inserted in TEST 9(keys 1,2,3) with one key.\n");
            fp << "         Expected Results:\n");
            fp << "            a. node is found with only key 0(value 1) and keys 1(value 2) and 2(value 3) are ignored.\n");
            fp << "               number of nodes remains at two since a search does not change the number of nodes.\n\n");

            fp << "TEST 10: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

            // a data node with these keys was inserted in TEST 9
            rdx1_key[0][0][NUM_KEY_BYTES] = 1;
            rdx1_key[0][1][NUM_KEY_BYTES] = 2;
            rdx1_key[0][2][NUM_KEY_BYTES] = 3;

            rdx1_key[0][0][0] = 1; // use first key(0)
            rdx1_key[0][1][0] = 0; // ignore second key(1)
            rdx1_key[0][2][0] = 0; // ignore third key(2)

            fp << "TEST 10: rdx_pat_search(&rdx1, rdx1_key[0]);\n");
            dnp = rdx_pat_search(&rdx1, rdx1_key[0]);
            if (dnp == NULL)
            {
                fp << "TEST 10: rdx_pat_search(&rdx1, rdx1_key[0]); FAIL: NULL return - search failed using only first(key 0 value 1) of three keys\n");
            }
            else
            {
                fp << "TEST 10: rdx_pat_search(&rdx1, rdx1_key[0]); - search using only first(key 0 value 1) of three keys succeeded\n\n");

                fp << "TEST 10: the *dnp data node keys:\n\n");
                print_keys(fp, dnp);
            }

            fp << "TEST 10: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
        }

        fp.close();
    }

    {
        if ( NUM_KEYS != 3 || MAX_NUM_RDX_NODES < 2 )
        {
            fp << "####################################################################################################\n");
            fp << "TEST 11: Not done - needs NUM_KEYS = 3 and MAX_NUM_RDX_NODES >= 2.\n");
        }
        else
        {
            fp << "####################################################################################################\n");
            fp << "TEST 11: Delete one data node in rdx1 inserted in TEST 9(keys 4,5,6) with one key.\n");
            fp << "         Expected Results:\n");
            fp << "            a. node is removed with only key 2(value 6) and keys 1(value 5) and 0(value 4) are ignored.\n");
            fp << "               number of nodes decreases by one because of the remove.\n\n");

            fp << "TEST 11: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

            // a data node with these keys was inserted in TEST 9
            rdx1_key[1][0][NUM_KEY_BYTES] = 4;
            rdx1_key[1][1][NUM_KEY_BYTES] = 5;
            rdx1_key[1][2][NUM_KEY_BYTES] = 6;

            rdx1_key[1][0][0] = 0; // ignore first key(0)
            rdx1_key[1][1][0] = 0; // ignore second key(1)
            rdx1_key[1][2][0] = 1; // use third key(2)

            fp << "TEST 11: rdx_pat_remove(&rdx1, rdx1_key[1]);\n");
            dnp = rdx_pat_remove(&rdx1, rdx1_key[1]);
            if (dnp == NULL)
            {
                fp << "TEST 11: rdx_pat_remove(&rdx1, rdx1_key[1]); FAIL: NULL return - remove failed using only third(key 2 value 6) of three keys\n");
            }
            else
            {
                fp << "TEST 11: rdx_pat_remove(&rdx1, rdx1_key[1]); - remove using only third(key 2 value 6) of three keys succeeded\n\n");

                fp << "TEST 11: the *dnp data node keys:\n\n");
                print_keys(fp, dnp);
            }

            fp << "TEST 11: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
        }

        fp.close();
    }

    {
        if ( NUM_KEYS != 3 || MAX_NUM_RDX_NODES < 2 )
        {
            fp << "####################################################################################################\n");
            fp << "TEST 12: Not done - needs NUM_KEYS = 3 and MAX_NUM_RDX_NODES >= 2.\n");
        }
        else
        {
            fp << "\n");
            fp << "TEST 12: Print one data node inserted in TEST 9(keys 1,2,3) with one key.\n");
            fp << "         Expected Results:\n");
            fp << "            a. node is printed with only key 1(value 2) and keys 0(value 1) and 2(value 3) are ignored\n\n");

            fp << "TEST 12: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

            // a data node with these keys was inserted in TEST 9
            rdx1_key[0][0][NUM_KEY_BYTES] = 1;
            rdx1_key[0][1][NUM_KEY_BYTES] = 2;
            rdx1_key[0][2][NUM_KEY_BYTES] = 3;

            rdx1_key[0][0][0] = 0; // ignore first key(0)
            rdx1_key[0][1][0] = 1; // use second key(1)
            rdx1_key[0][2][0] = 0; // ignore third key(2)

            fp << "TEST 12: rdx_pat_print(&rdx1, rdx1_key[0], fp);\n");
            rdx_pat_print(&rdx1, rdx1_key[0], fp);

            fp << "TEST 12: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
        }

        fp.close();
    }

    // initialize rdx2 PNODE structures
    rdx_size = rdx_pat_initialize(&rdx2);
    if ( rdx_size == 0 )
    {
        fp << "rdx_pat_initialize(&rdx2); FAIL\n");
        exit(1);
    }
    fp << "\n####################################################################################################\n");
    fp << "rdx2 size(bytes): %d\n", rdx_size);
    fp << "####################################################################################################\n\n");

    // in rdx2_key[][][] generate MAX_NUM_RDX_NODES+1 sets of NUM_KEYS random keys each of NUM_KEY_BYTES in length
    srand(time(NULL));
    for ( int n = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
    {
        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            rdx2_key[n][k][0] = 1; // set key boolean to 1
            for ( int b = 1 ; b < 1+NUM_KEY_BYTES ; b++ )
            {
                rdx2_key[n][k][b] = rand() & 0x000000ff;
            }
        }
    }

    {
        fp << "\n");
        fp << "TEST 13: Insert MAX_NUM_RDX_NODES+1 data nodes of NUM_KEYS random keys in rdx2 trie.\n");
        fp << "         Expected Results:\n");
        fp << "            a. MAX_NUM_RDX_NODES key insertions with return code 0\n");
        fp << "            b. 1 key insertion with return code 2(no free nodes)\n");
        fp << "            c. Total nodes allocated(not including root node) MAX_NUM_RDX_NODES\n");
        fp << "            d. No verification error\n\n");

        for ( int n = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
        {
            fp << "TEST 13: rdx_pat_insert(&rdx2, rdx2_key[%d], &dnp);\n", n);
            fp << "TEST 13: rdx2 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx2));
            return_code = rdx_pat_insert(&rdx2, rdx2_key[n], &dnp);

            fp << "TEST 13: rdx_pat_insert(&rdx2, rdx2_key[%d], &dnp); - Return code = %d\n\n", n, return_code);

            if ( return_code == 0 )
            {
                fp << "TEST 13: rdx_pat_insert(&rdx2, rdx2_key[%d], &dnp); - the *dnp keys are:\n\n", n);
                print_keys(fp, dnp);

                /*
                 * set some data in the APP_DATA struct of the data node - see APP_DATA.h.
                 * used 'aabbccdd' to be readily recognizable in the test output.
                 */
                dnp->data.id = 0xaabbccdd;
            }
        }

        fp.close();
    }

    {
        fp << "\n");
        fp << "TEST 14: Test rdx_pat_print() with NULL key argument on rdx2 trie\n");
        fp << "         Expected Results:\n");
        fp << "            a. Should print all branch and data nodes in rdx2 trie\n\n");

        fp << "TEST 14: rdx2 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx2));

        fp << "TEST 14: rdx_pat_print(&rdx2, NULL, fp);\n");
        rdx_pat_print(&rdx2, NULL, fp);

        fp << "TEST 14: rdx2 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx2));

        fp.close();
    }
#endif
}

