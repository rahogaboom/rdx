/*
 * test cases and example code for MKRdxPat.h class library routines
 *
 * NOTES:
 * 1. tell user to try variations on nodes, keys, key length of existing tests - much more flexible this way
 * 2. do TEST of really large nodes, keys, key length
 * 3. update man page doc in MKRdxPat.h
 * 4. test insert() one node and search() for same node with one key and remove() for same node with another key
 * 5. test with IPv4 and IPv6 keys - big
 * 6. cpplint everything
 */


#include <iostream>
#include <fstream>

//#define DEBUG
#include "MKRdxPat.h"

using namespace MultiKeyRdxPat;
using namespace std;


// print all NUM_KEYS keys in a data node
    void
print_keys
    (
        ofstream& os,
        DNODE *dnp,
        int NUM_KEYS,
        int NUM_KEY_BYTES
    )
{
    if ( dnp == NULL )
    {
        os << "Can't print data node keys: NULL data node\n";
    }
    else
    {
        char string[4];

        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            os << "   key " << k << " = ";
            for ( int b = 0 ; b < NUM_KEY_BYTES ; b++ )
            {
                sprintf(string, "%02x ", dnp->key[k*(1+NUM_KEY_BYTES) + b+1]);
                os << string;
            }
            os << endl;
        }
        os << endl;
    }
}


   int
main()
{
    { // TEST 0
        ofstream os;
        os.open("MKRdxPat.TEST0.results");

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 4;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        os << endl;
        os << "TEST 0: Print all allocated nodes plus the impossible key root node in rdx trie.\n";
        os << "        Expected Results:\n";
        os << "           a. Only the impossible pre-allocated root node should be printed since\n";
        os << "              no other nodes have been inserted\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << endl;
        os << "NUM_KEYS = " << NUM_KEYS << endl;
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat *rdx = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        rdx->print(NULL, os);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    { // TEST 1
        int return_code;

        ofstream os;
        os.open("MKRdxPat.TEST1.results");

        DNODE *dnp;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 4;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0,sum = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1; // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        os << endl;
        os << "TEST 1: Insert one data node with NUM_KEYS keys in rdx trie.\n";
        os << "        Expected Results:\n";
        os << "           a. One key insertion with return code 0\n";
        os << "           b. Total nodes allocated(not including root node) 1\n";
        os << "           c. No verification error\n";
        os << "           d. Print entire trie - allocated node and root node\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << endl;
        os << "NUM_KEYS = " << NUM_KEYS << endl;
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat *rdx = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        return_code = rdx->insert((unsigned char *)rdx_key[0], &dnp);

        os << "rdx->insert((unsigned char *)rdx_key[0], &dnp);  Return Code = " << return_code << endl;

        os << "the *dnp data node keys:\n";
        print_keys(os, dnp, NUM_KEYS, NUM_KEY_BYTES);

        /*
         * set some data in the APP_DATA struct of the data node - see APP_DATA.h.
         * this is the user application data and is set after rdx->insert() has returned
         * a pointer to a free data node with the keys now set to rdx_key[n].  open
         * APP_DATA.h and install the APP_DATA structure that you want and use dnp to
         * set the structure data.  only id is set here.  used id = 'aabbccdd' to be
         * readily recognizable in the test output.
         */
        dnp->data.id = 0xaabbccdd;

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os << "rdx->verify(ERR_CODE_PRINT, os); print diagnostics:\n\n";
        return_code = rdx->verify(ERR_CODE_PRINT, os);
        if ( return_code != 0 )
        {
            os << "rdx->verify(ERR_CODE_PRINT, os); verification error - " << return_code << endl;
        }
        else
        {
            os << "rdx->verify(ERR_CODE_PRINT, os); verification successful\n";
        }

        os << "rdx->print(NULL, os); print entire rdx trie:\n\n";
        rdx->print(NULL, os);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    { // TEST 2
        int return_code;

        ofstream os;
        os.open("MKRdxPat.TEST2.results");

        DNODE *dnp;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 4;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0,sum = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1; // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        os << endl;
        os << "TEST 2: Test rdx->print(key, os) for rdx trie - insert one data node first.\n";
        os << "        Expected Results:\n";
        os << "           a. Should print only the data node with the passed in keys and all of the branch\n";
        os << "              nodes leading to that data node.  If there are N keys in that data node then\n";
        os << "              there will be N branch node sequences that lead to the same data node\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << endl;
        os << "NUM_KEYS = " << NUM_KEYS << endl;
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat *rdx = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        return_code = rdx->insert((unsigned char *)rdx_key[0], &dnp);

        os << "rdx->insert((unsigned char *)rdx_key[" << 0 << "], &dnp); Return Code = " << return_code << endl;

        rdx->print((unsigned char *)rdx_key[0], os);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    { // TEST 3
        int return_code;

        ofstream os;
        os.open("MKRdxPat.TEST3.results");

        DNODE *dnp;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 6;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0,sum = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1; // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        os << endl;
        os << "TEST 3: Insert four data nodes each with NUM_KEYS keys in rdx trie.\n";
        os << "        Expected Results:\n";
        os << "           a. Four data node insertions with return code 0\n";
        os << "           b. Total nodes allocated(not including root node) 4\n";
        os << "           c. No verification error\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << endl;
        os << "NUM_KEYS = " << NUM_KEYS << endl;
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat *rdx = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        for ( int n = 0 ; n < 4 ; n++ )
        {
            return_code = rdx->insert((unsigned char *)rdx_key[n], &dnp);

            os << "rdx->insert((unsigned char *)rdx_key[" << n << "], &dnp); Return Code = " << return_code << endl;

            os << "the *dnp data node keys:\n";
            print_keys(os, dnp, NUM_KEYS, NUM_KEY_BYTES);

            if ( return_code == 0 )
            {
                /*
                 * set some data in the APP_DATA struct of the data node - see APP_DATA.h.
                 * used 'aabbccdd' to be readily recognizable in the test output
                 */
                dnp->data.id = 0xaabbccdd;
            }
        }
        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os << "call rdx->verify(ERR_CODE_PRINT, os); print diagnostics:\n\n";
        return_code = rdx->verify(ERR_CODE_PRINT, os);
        if ( return_code != 0 )
        {
            os << "rdx->verify(ERR_CODE_PRINT, os); verification error - " << return_code << endl;
        }
        else
        {
            os << "rdx->verify(ERR_CODE_PRINT, os); verification successful\n";
        }

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    { // TEST 4
        int return_code;

        ofstream os;
        os.open("MKRdxPat.TEST4.results");

        DNODE *dnp;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 8;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0,sum = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1; // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        os << endl;
        os << "TEST 4: Search for all MAX_NUM_RDX_NODES data nodes with NUM_KEYS keys in rdx trie.\n";
        os << "        Expected Results:\n";
        os << "           a. insert 4 data nodes and search/find them\n";
        os << "           b. fail to find 4 nodes who's keys were not inserted\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << endl;
        os << "NUM_KEYS = " << NUM_KEYS << endl;
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat *rdx = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        for ( int n = 0 ; n < 4 ; n++ )
        {
            return_code = rdx->insert((unsigned char *)rdx_key[n], &dnp);

            os << "rdx->insert((unsigned char *)rdx_key[" << n << "], &dnp); Return Code = " << return_code << endl;

            os << "the *dnp data node keys:\n";
            print_keys(os, dnp, NUM_KEYS, NUM_KEY_BYTES);
        }

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            dnp = rdx->search((unsigned char *)rdx_key[n]);
            if (dnp == NULL)
            {
                os << "rdx->search((unsigned char *)rdx_key[" << n << "]); NULL return - search fail\n";
            }
            else
            {
                os << "rdx->search((unsigned char *)rdx_key[" << n << "]);\n";

                os << "the *dnp data node keys:\n";
                print_keys(os, dnp, NUM_KEYS, NUM_KEY_BYTES);
            }
        }

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    { // TEST 5
        int return_code;

        ofstream os;
        os.open("MKRdxPat.TEST5.results");

        DNODE *dnp;
        DNODE **sorted_nodes; // used to test rdx->sort()

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 4;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0,sum = MAX_NUM_RDX_NODES*NUM_KEYS ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1; // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum--;
            }
        }

        os << endl;
        os << "TEST 5: Sort data nodes by successive keys in rdx trie\n";
        os << "        Expected Results:\n";
        os << "           a. For each key the return code will equal the number of sorted nodes and the\n";
        os << "              nodes array will hold the array of node pointers to nodes in sorted order\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << endl;
        os << "NUM_KEYS = " << NUM_KEYS << endl;
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat *rdx = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        for ( int n = 0,sum = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            const size_t MSG_BUF_SIZE = 256;
            char string[MSG_BUF_SIZE];

            return_code = rdx->insert((unsigned char *)rdx_key[n], &dnp);

            os << "rdx->insert((unsigned char *)rdx_key[" << n << "], &dnp); Return Code = " << return_code << endl;

            dnp->data.id = sum++;

            snprintf(string, MSG_BUF_SIZE, "n = %d  data.id = %08x\n", n, dnp->data.id);
            os << string;

            os << "the *dnp data node keys:\n";
            print_keys(os, dnp, NUM_KEYS, NUM_KEY_BYTES);
        }

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            os << "Sort the nodes by key " << k << ".\n\n";
            return_code = rdx->sort(&sorted_nodes, k);
            os << "rdx->sort(&sorted_nodes, " << k << "); Return code = " << return_code << endl;

            for ( int n = 0 ; n < return_code ; n++ )
            {
                if ( sorted_nodes[n] == NULL )
                {
                    os << "n = " << n << " NULL\n";
                }
                else
                {
                    const size_t MSG_BUF_SIZE = 256;
                    char string[MSG_BUF_SIZE];

                    snprintf(string, MSG_BUF_SIZE, "n = %d  data.id = %08x\n", n, ((DNODE *)sorted_nodes[n])->data.id);
                    os << string;

                    os << "the *dnp data node keys:\n";
                    print_keys(os, (DNODE *)sorted_nodes[n], NUM_KEYS, NUM_KEY_BYTES);
                }
            }
            os << "rdx - Total sorted nodes = " << return_code << "\n\n";
        }

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    { // TEST 6
        int return_code;

        ofstream os;
        os.open("MKRdxPat.TEST6.results");

        DNODE *dnp;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 8;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0,sum = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1; // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        os << endl;
        os << "TEST 6: Remove all keys in rdx trie\n";
        os << "        Expected Results:\n";
        os << "           a. Non-NULL returns of the nodes removed(5), NULL returns(3) for nodes not\n";
        os << "              allocated and zero allocated nodes upon completion\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << endl;
        os << "NUM_KEYS = " << NUM_KEYS << endl;
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat *rdx = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        for ( int n = 0,sum = 0 ; n < 5 ; n++ )
        {
            const size_t MSG_BUF_SIZE = 256;
            char string[MSG_BUF_SIZE];

            return_code = rdx->insert((unsigned char *)rdx_key[n], &dnp);

            os << "rdx->insert((unsigned char *)rdx_key[" << n << "], &dnp); Return Code = " << return_code << endl;

            dnp->data.id = sum++;

            snprintf(string, MSG_BUF_SIZE, "n = %d  data.id = %08x\n", n, dnp->data.id);
            os << string;

            os << "the *dnp data node keys:\n";
            print_keys(os, dnp, NUM_KEYS, NUM_KEY_BYTES);
        }

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            os << "rdx->remove((unsigned char *)rdx_key[" << n << "]);\n";
            dnp = rdx->remove((unsigned char *)rdx_key[n]);
            if (dnp == NULL)
            {
                os << "rdx->remove((unsigned char *)rdx_key[" << n << "]); NULL return - remove fail\n";
            }
            else
            {
                os << "rdx->remove((unsigned char *)rdx_key[" << n << "]); remove successful\n";

                os << "the *dnp data node keys:\n";
                print_keys(os, dnp, NUM_KEYS, NUM_KEY_BYTES);
            }
        }
        os << endl;

        os << "rdx->verify(ERR_CODE_PRINT, os); print diagnostics:\n";
        return_code = rdx->verify(ERR_CODE_PRINT, os);
        if ( return_code != 0 )
        {
            os << "rdx->verify(ERR_CODE_PRINT, os); verification error - " << return_code << endl;
        }
        else
        {
            os << "rdx->verify(ERR_CODE_PRINT, os); verification successful\n\n";
        }

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    { // TEST 7
        int return_code;

        ofstream os;
        os.open("MKRdxPat.TEST7.results");

        DNODE *dnp;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 8;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));

        os << endl;
        os << "TEST 7: Insert/Search/Remove MAX_NUM_RDX_NODES nodes with keys repeatedly.\n";
        os << "        Expected Results:\n";
        os << "           a. Do not report success - report only errors in insert/search/remove operations\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << endl;
        os << "NUM_KEYS = " << NUM_KEYS << endl;
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat *rdx = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        int tot_errs;
        int test_num;

        for ( test_num = 0,tot_errs = 0 ; test_num < 32 ; test_num++ )
        {
            // in rdx_key[][][] generate MAX_NUM_RDX_NODES sets of NUM_KEYS keys each of NUM_KEY_BYTES in length
            srand(time(NULL));
            for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
            {
                for ( int k = 0 ; k < NUM_KEYS ; k++ )
                {
                    rdx_key[n][k][0] = 1; // set key boolean to 1
                    for ( int b = 1 ; b < 1+NUM_KEY_BYTES ; b++ )
                    {
                        rdx_key[n][k][b] = rand() & 0x000000ff;
                    }
                }
            }

            // insert full set of MAX_NUM_RDX_NODES data nodes
            for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
            {
                os << "rdx->insert((unsigned char *)rdx_key[" << n << "], &dnp);\n";
                return_code = rdx->insert((unsigned char *)rdx_key[n], &dnp);
                if ( return_code != 0 )
                {
                    os << "rdx->insert((unsigned char *)rdx_key[" << n << "], &dnp); error - " << return_code << endl;
                    tot_errs++;
                }
            }
            os << endl;

            // search for full set of MAX_NUM_RDX_NODES data nodes
            for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
            {
                os << "rdx->search((unsigned char *)rdx_key[" << n << "]);\n";
                dnp = rdx->search((unsigned char *)rdx_key[n]);
                if (dnp == NULL)
                {
                    os << "rdx->search((unsigned char *)rdx_key[" << n << "]); error\n";
                    tot_errs++;
                }
            }
            os << endl;

            // remove a full set of MAX_NUM_RDX_NODES data nodes
            for ( int n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
            {
                os << "rdx->remove((unsigned char *)rdx_key[" << n << "]);\n";
                dnp = rdx->remove((unsigned char *)rdx_key[n]);
                if (dnp == NULL)
                {
                    os << "rdx->remove((unsigned char *)rdx_key[" << n << "]); error\n";
                    tot_errs++;
                }
            }

            os << "test = " << test_num << "  tot_errs = " << tot_errs << endl;
            os << "\n\n";
        }
        os << " Total Insert/Search/Remove tests run " << test_num << "  Total errors detected " << tot_errs << "\n\n";

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        os.close();
    }

    { // TEST 8
        int return_code;

        ofstream os;
        os.open("MKRdxPat.TEST8.results");

        DNODE *dnp;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 1;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, MAX_NUM_RDX_NODES * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0,sum = 3 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1; // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        os << endl;
        os << "TEST 8: Insert one data node in rdx with specific keys - key 0 = 3, key 1 = 4, key 2 = 5\n";
        os << "        Do, using only a single key, searches/removes with correct/incorrect keys\n";
        os << "        Expected Results:\n";
        os << "           a. search on the inserted data node - should succeed\n";
        os << "           b. search using only one correct key - should succeed\n";
        os << "           c. search using only one incorrect key - should fail\n";
        os << "           d. remove using only one incorrect key - should fail\n";
        os << "           e. remove using only one correct key - should succeed\n";
        os << "           f. print using only one key - should succeed\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << endl;
        os << "NUM_KEYS = " << NUM_KEYS << endl;
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat *rdx = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        // key booleans:
        rdx_key[0][0][0] = 1;  // use first key 0 = 3
        rdx_key[0][1][0] = 1;  // use second key 1 = 4
        rdx_key[0][2][0] = 1;  // use third key 2 = 5
        os << "// key booleans:\n";
        os << "rdx_key[0][0][0] = 1;  // use first key 0 = 3\n";
        os << "rdx_key[0][1][0] = 1;  // use second key 1 = 4\n";
        os << "rdx_key[0][2][0] = 1;  // use third key 2 = 5\n";
        return_code = rdx->insert((unsigned char *)rdx_key[0], &dnp);
        os << "rdx->insert((unsigned char *)rdx_key[" << 0 << "], &dnp); Return Code = " << return_code << "\n\n";

        os << "the *dnp data node keys:\n";
        print_keys(os, dnp, NUM_KEYS, NUM_KEY_BYTES);

        os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";

        // key booleans:
        rdx_key[0][0][0] = 1;  // use first key 0 = 3
        rdx_key[0][1][0] = 1;  // use second key 1 = 4
        rdx_key[0][2][0] = 1;  // use third key 2 = 5
        os << "// key booleans:\n";
        os << "rdx_key[0][0][0] = 1;  // use first key 0 = 3\n";
        os << "rdx_key[0][1][0] = 1;  // use second key 1 = 4\n";
        os << "rdx_key[0][2][0] = 1;  // use third key 2 = 5\n";
        dnp = rdx->search((unsigned char *)rdx_key[0]);
        if (dnp == NULL)
        {
            os << "rdx->search((unsigned char *)rdx_key[0]); search fail - unexpected.\n";
        }
        else
        {
            os << "rdx->search((unsigned char *)rdx_key[0]); search succeeds - expected.\n";
        }
        os << endl;

        // key booleans:
        rdx_key[0][0][0] = 0;  // ignore first key 0 = 3
        rdx_key[0][1][0] = 0;  // ignore second key 1 = 4
        rdx_key[0][2][0] = 1;  // use third key 2 = 5
        os << "// key booleans:\n";
        os << "rdx_key[0][0][0] = 0;  // ignore first key 0 = 3\n";
        os << "rdx_key[0][1][0] = 0;  // ignore second key 1 = 4\n";
        os << "rdx_key[0][2][0] = 1;  // use third key 2 = 5\n";
        dnp = rdx->search((unsigned char *)rdx_key[0]);
        if (dnp == NULL)
        {
            os << "rdx->search((unsigned char *)rdx_key[0]); search fail - unexpected.\n";
        }
        else
        {
            os << "rdx->search((unsigned char *)rdx_key[0]); search succeeds - expected.\n";
        }
        os << endl;

        // key booleans:
        rdx_key[0][0][0] = 0;  // ignore first key 0 = 3
        rdx_key[0][1][0] = 0;  // ignore second key 1 = 4
        rdx_key[0][2][0] = 1;  // use third key 2 = 6 - wrong
        rdx_key[0][2][NUM_KEY_BYTES] = 6;  // use third key 2 = 6 - wrong
        os << "// key booleans:\n";
        os << "rdx_key[0][0][0] = 0;  // ignore first key 0 = 3\n";
        os << "rdx_key[0][1][0] = 0;  // ignore second key 1 = 4\n";
        os << "rdx_key[0][2][0] = 1;  // use third key 2 = 6 - wrong\n";
        os << "rdx_key[0][2][NUM_KEY_BYTES] = 6;  // use third key 2 = 6 - wrong\n";
        dnp = rdx->search((unsigned char *)rdx_key[0]);
        if (dnp == NULL)
        {
            os << "rdx->search((unsigned char *)rdx_key[0]); search fail - expected.\n";
        }
        else
        {
            os << "rdx->search((unsigned char *)rdx_key[0]); search succeeds - unexpected.\n";
        }
        os << endl;

        // key booleans:
        rdx_key[0][0][0] = 0;  // ignore first key 0 = 3
        rdx_key[0][1][0] = 1;  // use second key 1 = 4
        rdx_key[0][2][0] = 0;  // ignore third key 2 = 5
        os << "// key booleans:\n";
        os << "rdx_key[0][0][0] = 0;  // ignore first key 0 = 3\n";
        os << "rdx_key[0][1][0] = 1;  // use second key 1 = 4\n";
        os << "rdx_key[0][2][0] = 0;  // ignore third key 2 = 5\n";
        return_code = rdx->print((unsigned char *)rdx_key[0], os);

        if ( return_code == 0 )
        {
            os << "rdx->print((unsigned char *)rdx_key[0], os); print success - expected.\n";
        }
        else
        {
            os << "rdx->print((unsigned char *)rdx_key[0], os); print fail - unexpected.\n";
        }
        os << endl;

        // key booleans:
        rdx_key[0][0][0] = 0;  // ignore first key 0 = 3
        rdx_key[0][1][0] = 0;  // ignore second key 1 = 4
        rdx_key[0][2][0] = 1;  // use third key 2 = 6 - wrong
        rdx_key[0][2][NUM_KEY_BYTES] = 6;  // use third key 2 = 6 - wrong
        os << "// key booleans:\n";
        os << "rdx_key[0][0][0] = 0;  // ignore first key 0 = 3\n";
        os << "rdx_key[0][1][0] = 0;  // ignore second key 1 = 4\n";
        os << "rdx_key[0][2][0] = 1;  // use third key 2 = 6 - wrong\n";
        os << "rdx_key[0][2][NUM_KEY_BYTES] = 6;  // use third key 2 = 6 - wrong\n";
        dnp = rdx->remove((unsigned char *)rdx_key[0]);
        if (dnp == NULL)
        {
            os << "rdx->remove((unsigned char *)rdx_key[0]); remove fail - expected.\n";
        }
        else
        {
            os << "rdx->remove((unsigned char *)rdx_key[0]); remove succeeds - unexpected.\n";
        }
        os << endl;

        // key booleans:
        rdx_key[0][0][0] = 0;  // ignore first key 0 = 3
        rdx_key[0][1][0] = 0;  // ignore second key 1 = 4
        rdx_key[0][2][0] = 1;  // use third key 2 = 5
        rdx_key[0][2][NUM_KEY_BYTES] = 5;  // use third key 2 = 5
        os << "// key booleans:\n";
        os << "rdx_key[0][0][0] = 0;  // ignore first key 0 = 3\n";
        os << "rdx_key[0][1][0] = 0;  // ignore second key 1 = 4\n";
        os << "rdx_key[0][2][0] = 1;  // use third key 2 = 5\n";
        os << "rdx_key[0][2][NUM_KEY_BYTES] = 5;  // use third key 2 = 5\n";
        dnp = rdx->remove((unsigned char *)rdx_key[0]);
        if (dnp == NULL)
        {
            os << "rdx->remove((unsigned char *)rdx_key[0]); remove fail - unexpected.\n";
        }
        else
        {
            os << "rdx->remove((unsigned char *)rdx_key[0]); remove succeeds - expected.\n";
        }
        os << endl;

        os.close();
    }

    { // TEST 9
        int return_code;

        ofstream os;
        os.open("MKRdxPat.TEST9.results");

        DNODE *dnp;

        // maximum number of data nodes stored in rdx trie
        const int MAX_NUM_RDX_NODES = 4;

        // number of rdx search keys
        const int NUM_KEYS = 3;

        // number of bytes in each key(s)
        const int NUM_KEY_BYTES = 4;

        // 1+MAX_NUM_RDX_NODES nodes of NUM_KEYS keys of NUM_KEY_BYTES bytes - set all key booleans to 1
        unsigned char rdx_key[MAX_NUM_RDX_NODES+1][NUM_KEYS][1+NUM_KEY_BYTES];

        memset(rdx_key, 0, (MAX_NUM_RDX_NODES+1) * NUM_KEYS * (1+NUM_KEY_BYTES));
        for ( int n = 0,sum = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
        {
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                rdx_key[n][k][0] = 1; // set key boolean to 1
                rdx_key[n][k][NUM_KEY_BYTES] = sum++;
            }
        }

        os << endl;
        os << "TEST 9: Insert MAX_NUM_RDX_NODES+1 data nodes in a rdx trie of only MAX_NUM_RDX_NODES nodes.\n";
        os << "        Expected Results:\n";
        os << "           a. MAX_NUM_RDX_NODES node insertions with return code 0\n";
        os << "           b. 1 node insertion with return code 2(no free nodes)\n";
        os << "           c. Total nodes allocated(not including root node) MAX_NUM_RDX_NODES\n";
        os << "           d. No verification error\n\n";

        os << "MAX_NUM_RDX_NODES = " << MAX_NUM_RDX_NODES << endl;
        os << "NUM_KEYS = " << NUM_KEYS << endl;
        os << "NUM_KEY_BYTES = " << NUM_KEY_BYTES << "\n\n";

        MKRdxPat *rdx = new MKRdxPat(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

        for ( int n = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
        {
            os << "rdx->insert((unsigned char *)rdx_key[" << n << "], &dnp);\n";
            os << "rdx - Nodes allocated = " << rdx->nodes() << "\n\n";
            return_code = rdx->insert((unsigned char *)rdx_key[n], &dnp);

            os << "rdx->insert((unsigned char *)rdx_key[" << n << "], &dnp); Return Code = " << return_code << endl;

            if ( return_code == 0 )
            {
                os << "rdx->insert((unsigned char *)rdx_key[" << n << "], &dnp); the *dnp keys are:\n";
                print_keys(os, dnp, NUM_KEYS, NUM_KEY_BYTES);
            }
        }
        os << endl;

        os << "rdx->verify(ERR_CODE_PRINT, os); print diagnostics:\n\n";
        return_code = rdx->verify(ERR_CODE_PRINT, os);
        if ( return_code != 0 )
        {
            os << "rdx->verify(ERR_CODE_PRINT, os); verification error - " << return_code << endl;
        }
        else
        {
            os << "rdx->verify(ERR_CODE_PRINT, os); verification successful\n";
        }
        os << endl;

        os.close();
    }
}

