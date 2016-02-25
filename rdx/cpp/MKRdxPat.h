#ifndef RDX_CPP_MKRDXPAT_H_
#define RDX_CPP_MKRDXPAT_H_

// Copyright 2015-2016 Richard A Hogaboom  [legal/copyright]

/*
 *======================================================================================================================
 *
 * File Names:
 *     MKRdxPat.h
 *
 *======================================================================================================================
 *
 * Member Functions:
 *
 *     struct app_data
 *     {
 *         int data;  // user specified application data structure
 *     };
 *
 *     MKRdxPat<app_data>
 *         (
 *             int MAX_NUM_RDX_NODES,
 *             int NUM_KEYS,
 *             int NUM_KEY_BYTES
 *         );
 *         e.g. MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(512, 3, 4);
 *
 *
 *     ~MKRdxPat()
 *         e.g. delete rdx;
 *
 *
 *     struct app_data
 *     {
 *         int data;  // user specified application data structure
 *     };
 *
 *         int
 *     insert
 *         (
 *             unsigned char *key,  // unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
 *             app_data **app_datapp
 *         );
 *         e.g. return_code = rdx->insert((unsigned char *)key, &app_datap);
 *
 *
 *     struct app_data
 *     {
 *         int data;  // user specified application data structure
 *     };
 *
 *         app_data *
 *     search
 *         (
 *             unsigned char *key  // unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
 *         );
 *         e.g. app_datap = rdx->search((unsigned char *)key);
 *
 *
 *     struct app_data
 *     {
 *         int data;  // user specified application data structure
 *     }
 *
 *         app_data *
 *     remove
 *         (
 *             unsigned char *key  // unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
 *         );
 *         e.g. app_datap = rdx->remove((unsigned char *)key);
 *
 *
 *         int
 *     sort
 *         (
 *             app_data ***app_datappp,
 *             int k
 *         );
 *         e.g. return_code = rdx->sort(&app_datapp, k);
 *
 *
 *         int
 *     nodes
 *         (
 *         );
 *         e.g. nodes = rdx->nodes();
 *
 *
 *         int
 *     print
 *         (
 *             unsigned char *key,  // unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
 *             ofstream& os
 *         );
 *         e.g. return_code = rdx->print(NULL, os);
 *         e.g. return_code = rdx->print((unsigned char *)key, os);
 *
 *
 *         int
 *     verify
 *         (
 *             VERIFY_MODE vm,
 *             ofstream& os
 *         );
 *         e.g. return_code = rdx->verify(ERR_CODE, os);
 *         e.g. return_code = rdx->verify(ERR_CODE_PRINT, os);
 *
 *======================================================================================================================
 *
 * Description:
 *     The MKRdxPat class allows the allocation of a fixed sized contiguous data store that holds data nodes of
 *     an arbitrary structure that may be accessed with any number of keys of any size with the PATRICIA (Practical
 *     Algorithm To Retrieve Information Coded In Alphanumeric)(1,2,3) fast search algorithm.  MKRdxPat is particularly
 *     suited to applications the require complex data structures be contiguously stored and accessed with an algorithm
 *     of known fast character with any of several possible keys, either singly of several simultaneously.
 *
 *     For example, a data structure that required either of an IPv4, IPv6 or MAC address key(s) to access data nodes.
 *     In this case NUM_KEYS would be 3 and NUM_KEY_BYTES would be 16(the longest - see below).
 *
 *     The MKRdxPat class supports a data structure of MAX_NUM_RDX_NODES data nodes and NUM_KEYS keys per data node
 *     with keys of NUM_KEY_BYTES bytes.  The class constructor:
 *
 *         MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);
 *
 *     along with a:
 *
 *         struct app_data
 *         {
 *             // user data
 *         };
 *
 *     class constructor typename specifies the data structure.  A data node may be accessed with a single key or any
 *     number of keys simultaneously.  Keys must be unique within their key index(0 - NUM_KEYS-1), but not over
 *     different key indexes.  Member functions that require keys are passed the array:
 *
 *         unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
 *
 *     One extra prefix byte, a key boolean - required to be 0(ignore) or 1(search), is set with each key to specify
 *     if the key is to be used in the data node search.  It is suggested that the array be first memset() to 0:
 *
 *         memset(key, 0, NUM_KEYS * (1+NUM_KEY_BYTES));
 *
 *     and then only keys to be used copied in with the key boolean(set to 1) prepended.
 *
 *     In most cases the necessary keys will be of different lengths.  The NUM_KEY_BYTES constructor argument would be
 *     set to the longest of these.  All other shorter keys would be left justified in the key[][] array, that is,
 *     start just after the key boolean.  Thus, for the example above, the IPv6 key would use the full 16 bytes, the
 *     IPv4 key would use the first four bytes after the key boolean(with the rest(12) set to 0) and the MAC key would
 *     use the first 6 bytes after the key boolean(with the rest(10) set to 0).  This was done to make the specification
 *     of the key[][] array and the code associated with processing it simpler.  Example:
 *
 *                 kb key bytes
 *         IPv4 :  01 aa bb cc dd 00 00 00 00 00 00 00 00 00 00 00 00
 *         IPv6 :  01 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp
 *         MAC  :  01 aa bb cc dd ee ff 00 00 00 00 00 00 00 00 00 00
 *
 *     See the member function summary above or a detailed member function usage comment before each function's code.
 *     A verify() member function is provided that does extensive data structure memory analysis and a print() member
 *     function is provided that prints the structural details of all the branch and data nodes.
 *
 *     For a trie of NUM_KEYS keys, each data node will have NUM_KEYS branch nodes associated with it.  The number of
 *     actual data nodes in the data structure is MAX_NUM_RDX_NODES+1.  This extra node is for the initial branch nodes
 *     and initial data node with the impossible key of all 0xff's(1,2,3).  The number of user storable nodes is
 *     MAX_NUM_RDX_NODES.   The user, through the constructor, declares PNODEs, each of which contains a radix PATRICIA
 *     trie of MAX_NUM_RDX_NODES nodes with NUM_KEYS keys of NUM_KEY_BYTES bytes length.
 *
 *     These routines are a modification of the algorithm cited.  Specifically, the upward pointers used to terminate
 *     search are not used, and are instead used to point to data nodes as trie leaves.  In multi-key search with
 *     PATRICIA the upward pointer scheme will not work since each key would have pointed to a different branch node
 *     with it's associated data structure.  In multi-key PATRICIA search all keys must lead to the same data node.
 *     The viability of multi-key radix PATRICIA search depends on the fact that each data node insertion requires
 *     only one new branch node for each key, thus the initial allocation of NUM_KEYS branch nodes with each data node.
 *     This implementation has several additional branch and data node members not used in the original PATRICIA search
 *     algorithm.  These are for various purposes; related to changes in the original algorithm for multi-key search,
 *     related to changes in the original algorithm for leaf data nodes instead of upward pointers to data structures
 *     stored in branch nodes, related to printing, sorting and debugging.
 *
 *======================================================================================================================
 *
 * Operational Notes:
 *
 *     1. the first member of BNODE and DNODE typedefs, id, should not be moved.  when doing operations on the trie, a
 *        search will reference the id from a BNODE type initially and then terminate at a DNODE type.
 *
 *     2. the maximum number of key bits is MAX_KEY_BITS.  the impossible key in the initially allocated data node must
 *        be at least one bit longer.  this requires the data node storage for the keys to have one extra byte.
 *
 *     3. all keys are the same length - NUM_KEY_BYTES bytes.  the performance hit, the small memory savings and the
 *        added code complexity of having individually sized keys did not seem worth it.  thus, the longest required key
 *        will determine NUM_KEY_BYTES.  shorter keys may be left justified in the NUM_KEY_BYTES bytes.
 *
 *     4. it is suggested that the 'unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]' array be first memset() to 0 and then
 *        all the keys to be used with key boolean 1 added in one operation(keys with key boolean 0 are never needed).
 *        e.g. memset(key, 0, NUM_KEYS * (1+NUM_KEY_BYTES));
 *
 *     5. multidemensional arrays in C++ are implemented by subscripting calloc()'ed memory with parameterized values
 *        calculated from the static C array versions.  One, two and three dimensional arrays are illustrated as
 *        follows(see verify() for actual usage):
 *
 *        n -> subscripts max_num_rdx_nodes_
 *        k -> subscripts num_keys_
 *        b -> subscripts num_key_bytes_:
 *
 *        a. one dimensional:
 *           unsigned long dnode_addrs[max_num_rdx_nodes_+1];
 *               -> dnode_addrs[n]
 *
 *        b. two dimensional:
 *           unsigned long bnode_addrs[num_keys_][max_num_rdx_nodes_+1];
 *               -> bnode_addrs[k*(max_num_rdx_nodes_+1)+n]
 *
 *        c. three dimensional:
 *           unsigned char dnode_keys[num_keys_][max_num_rdx_nodes_+1][num_key_bytes_];
 *               -> dnode_keys[(k*(max_num_rdx_nodes_+1)+n)*num_key_bytes_+b]
 *
 *======================================================================================================================
 *
 * Theoretic Notes:
 *
 *     1. depending on the key structure it is possible for the trie to be very unbalanced.  for a three byte key, for
 *        example, if 24 keys are inserted, each with only one different bit set, then the resulting trie would be a
 *        string of branch nodes extending to the left, each with a data node on its right branch.  the maximum trie
 *        depth would be 24 with the key of all zeros.
 *
 *     2. for the N node trie with b bit keys, even for very long keys, the average search requires log2(N) bit
 *        comparisons, and the average trie depth is log2(N).  see reference 1.  for a full key space, N = 2**b,
 *        log2(2**b) = b, and the average trie depth approaches b.
 *
 *     3. note that insertion requires two traversals of the trie - one to find if the key(s) already exists and the
 *        other to do the actual insertion.  the user may think that there must be a way to traverse down the trie only
 *        once to do an insertion.  the answer is YES! But.  in order to know where to stop you need to know all the
 *        key bits that intervene between the two b bit parameters of each branch node.  thus you would have to store
 *        these bits in each branch node or you could store any full key from any data node below a branch node in that
 *        branch node, which, by the property of a PATRICIA trie, will all have prefix bits identical for all data
 *        nodes below that branch node.  if, on insertion, the insertion key had bits differing between the two b bit
 *        parameters of two branch nodes then an insertion would be done at that point.  all this key storage and bit
 *        comparison in each traversed branch node is inefficient.  the first search traversal finds the key that does
 *        not match, but this key has all the key bits needed to find the first key bit of the new key that is
 *        different from the nearest key already in the trie.  this first differing bit then tells the second traversal
 *        where to stop and insert the new key.  thus, dual traversal is more efficient and simpler.
 *
 *======================================================================================================================
 *
 * References:
 *
 *     1. Algorithms in C, Robert Sedgewick, Addison-Wesley, 1998,
 *        Patricia Tries, pp. 623-631.
 *
 *     2. PATRICIA - Practical Algorithm To Retrieve Information Coded in Alphanumeric, Donald R. Morrison,
 *        Journal of the Association for Computing Machinery, Vol. 15, No. 4, Oct. 1968, pp. 514-534.
 *
 *     3. The Art of Computer Programming: Sorting and Searching, Donald E. Knuth,
 *        Addison-Wesley, 1973, pp 490-499.
 *
 *======================================================================================================================
 *
 * Author: Richard Hogaboom
 *         richard.hogaboom@gmail.com
 *
 *======================================================================================================================
 */

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cstring>

namespace std {}
using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::ofstream;

namespace MultiKeyRdxPat
{
    // verify_mode arg to verify()
    typedef enum verify_mode
    {
        ERR_CODE,            // return 0 for no error and positive int with error code for any errors
        ERR_CODE_PRINT       // return same error code above but also print current data structure data
                             // node addresses/keys, branch node addresses and error messages
    } VERIFY_MODE;

    template <typename app_data> class MKRdxPat
    {
        private:
            const int max_num_rdx_nodes_;
            const int num_keys_;
            const int num_key_bytes_;

            // branch node typedef
            typedef struct bnode
            {
                unsigned int id;     // IDentification(id=0) for branch node - must be first field in nod
                unsigned int br;     // br=0: from left parent BRanch - br=1: from right parent branch
                void *p;             // ptr to Parent node
                unsigned int nsn;    // Node Sequence Number - 0->max_num_rdx_nodes_
                unsigned int b;      // Bit number to do branch test on(bits start at 0 from right to left)
                void *l, *r;         // Left and Right node ptrs
            } BNODE;

            // data node typedef for application declarations.  the key must have one extra byte from num_key_bytes_
            // specified above in order to have an "impossible" key data node set at initialization
            typedef struct dnode
            {
                unsigned int id;     // IDentification(id=1) for data node - must be first field in node
                unsigned int *br;    // br=0: from left parent BRanch - br=1: from right parent branch - br[num_keys_]
                BNODE **p;           // ptr to Parent node - p[num_keys_]
                unsigned int nsn;    // Node Sequence Number - 0->max_num_rdx_nodes_
                void *nnfp;          // Next Node Free Ptr
                unsigned int alloc;  // 1 - allocated in rdx trie, 0 - on free queue
                unsigned char *key;  // search KEY(s) - key[num_keys_][1+num_key_bytes_]
                app_data data;       // user defined data structure
            } DNODE;

            // typedef of struct for PATRICIA node holding max_num_rdx_nodes_ data nodes with num_keys_ keys of length num_key_bytes_
            typedef struct pnode
            {
                // total number of nodes alloc(does not include root(0xff key) node)
                unsigned int tot_nodes;

                // head of rdx search node trie for each key
                BNODE **head;  // BNODE *head[num_keys_]

                // array holding ptrs to app_data used in sort()
                app_data **app_data_ptrs;  // app_data *app_data_ptrs[max_num_rdx_nodes_+1]

                // number of ptrs in node_ptrs array(not including root node)
                unsigned int app_data_ptrs_cnt;

                // branch nodes and free queue head ptr(s) of branch nodes
                BNODE *bnodes;  // BNODE bnodes[max_num_rdx_nodes_+1][num_keys_]
                BNODE **bfree_head;  // BNODE *bfree_head[num_keys_]

                // data nodes and free queue head ptr of data nodes
                DNODE *dnodes;  // DNODE dnodes[max_num_rdx_nodes_+1]
                DNODE *dfree_head;
            } PNODE_;

            // free pointers for all calloc()s
            vector<void *> free_ptr_;

            // free msgs for all calloc()s
            vector<string> free_msg_;

            // primary data structure
            PNODE_ rdx_;

            // given a key and bit number (bits numbered 0 from right) will return that bit
            // used by: insert(), search(), remove(), print()
                unsigned int
            gbit
                (
                    unsigned char *key,
                    unsigned int bit_num
                )
            {
                unsigned int byte;
                unsigned int bit;
                unsigned char mask;

                // the byte index set here assumes one extra prefix byte in the input parameter key(for the 0xff of the root
                // node impossible key). thus, data nodes with keys of num_key_bytes_ will have a 0 byte prefix added, and the
                // byte index set here is not 0 to num_key_bytes_ but 1 to num_key_bytes_+1. e.g. if num_key_bytes_=1 and bit_num=0
                // then byte is set to 1 not 0.  if num_key_bytes_=16 and bit_num=0 then byte is set to 16 not 15.
                mask = 1;
                byte = num_key_bytes_ - bit_num/8;
                mask <<= bit_num%8;
                bit = key[byte] & mask;
                bit >>= bit_num%8;

                return bit;
            }  // gbit()

            // recursive routine to descend rdx_ trie
            // used by: sort()
                void
            recursive
                (
                    BNODE *bnode_ptr
                )
            {
                // bnode_ptr is NULL only at end of trie traversal
                if ( bnode_ptr == NULL )
                {
                    return;
                }

                // if branch node continue down trie.  if data node inc count and store node ptr
                if ( bnode_ptr->id != 1 )
                {
                    recursive( (BNODE *)(bnode_ptr->l) );
                    recursive( (BNODE *)(bnode_ptr->r) );
                }
                else
                {
                    rdx_.app_data_ptrs[rdx_.app_data_ptrs_cnt++] = &( ((DNODE *)(bnode_ptr))->data );
                }
            }  // recursive()

            // used to compare node addresses stored in unsigned longs
            // used by: verify()
                static int
            unsigned_long_compare
                (
                    const void *a,
                    const void *b
                )
            {
                if ( *(unsigned long *)a < *(unsigned long *)b )
                {
                    return -1;
                }
                else if ( *(unsigned long *)a > *(unsigned long *)b )
                {
                    return 1;
                }

                return 0;
            }  // unsigned_long_compare()

            /*
             *======================================================================================================================
             *     initialize()
             *
             * Purpose: 
             *     Initialize the defined PNODE_ data structure.  This includes setting the data and branch node sequence numbers
             *     and data node allocated status(to 0).  Set total allocated nodes to 0, and allocated status of root node to 1.
             *     Set the branch node free list up for each key and the data node free list.
             *
             * Usage:
             *     initialize();
             *
             * Returns:
             *     None
             *
             * Parameters:
             *     None
             *
             * Comments:
             *     1. remember that the data structure has 1+max_num_rdx_nodes_ nodes(each of which has one data and num_keys_
             *        branch nodes).  the first is the always allocated impossible key root node.  the rest are the
             *        max_num_rdx_nodes_ user usable nodes.
             *
             *     2. certain pointers in the root node, such as parent pointers, are set to zero and permanently remain so since
             *        they have nothing to point to.  in root node printouts they should be zero.
             */

                void
            initialize
                (
                )
            {
                // set data and branch node sequence numbers and node allocated status
                for ( int n = 0 ; n < max_num_rdx_nodes_+1 ; n++ )
                {
                    for ( int k = 0 ; k < num_keys_ ; k++ )
                    {
                        rdx_.bnodes[n*num_keys_+k].nsn = n;
                    }

                    rdx_.dnodes[n].nsn = n;
                    rdx_.dnodes[n].alloc = 0;
                }
                rdx_.dnodes[0].alloc = 1;  // root impossible key(0xff) node

                // reset total allocated nodes
                rdx_.tot_nodes = 0;

                // for each key set various branch/data/head fields and pointers
                for ( int k = 0 ; k < num_keys_ ; k++ )
                {
                    // set rdx trie head pointer(s)
                    rdx_.head[k] = &(rdx_.bnodes[0*num_keys_+k]);

                    // init head branch node
                    rdx_.bnodes[0*num_keys_+k].id = 0;
                    rdx_.bnodes[0*num_keys_+k].br = 0;
                    rdx_.bnodes[0*num_keys_+k].p = NULL;
                    rdx_.bnodes[0*num_keys_+k].b = num_key_bytes_*8;
                    rdx_.bnodes[0*num_keys_+k].l = &rdx_.dnodes[0];
                    rdx_.bnodes[0*num_keys_+k].r = NULL;

                    // build the branch node free list
                    rdx_.bfree_head[k] = &(rdx_.bnodes[1*num_keys_+k]);
                    for ( int n = 1 ; n < max_num_rdx_nodes_ ; n++ )
                    {
                        rdx_.bnodes[n*num_keys_+k].id = 0;
                        rdx_.bnodes[n*num_keys_+k].br = 0;
                        rdx_.bnodes[n*num_keys_+k].p = &rdx_.bnodes[(n+1)*num_keys_+k];
                        rdx_.bnodes[n*num_keys_+k].b = 0;
                        rdx_.bnodes[n*num_keys_+k].l = NULL;
                        rdx_.bnodes[n*num_keys_+k].r = NULL;
                    }
                    rdx_.bnodes[max_num_rdx_nodes_*num_keys_+k].id = 0;
                    rdx_.bnodes[max_num_rdx_nodes_*num_keys_+k].br = 0;
                    rdx_.bnodes[max_num_rdx_nodes_*num_keys_+k].p = NULL;
                    rdx_.bnodes[max_num_rdx_nodes_*num_keys_+k].b = 0;
                    rdx_.bnodes[max_num_rdx_nodes_*num_keys_+k].l = NULL;
                    rdx_.bnodes[max_num_rdx_nodes_*num_keys_+k].r = NULL;

                    // init data node fields
                    for ( int n = 0 ; n < max_num_rdx_nodes_+1 ; n++ )
                    {
                        rdx_.dnodes[n].br[k] = 0;
                        rdx_.dnodes[n].p[k] = 0;
                        memset( &rdx_.dnodes[n].key[k*(1+num_key_bytes_)], 0, num_key_bytes_+1 );
                    }

                    // set data node parent pointers of root node to branch nodes for each key
                    rdx_.dnodes[0].p[k] = &rdx_.bnodes[0*num_keys_+k];

                    // init head data node key to impossible value, (the extra high order 0xff byte).
                    memset( &rdx_.dnodes[0].key[k*(1+num_key_bytes_)], 0xff, num_key_bytes_+1 );
                }

                // build the data node free list - use DNODE nnfp as pointer to next node
                rdx_.dnodes[0].id = 1;

                rdx_.dfree_head = &rdx_.dnodes[1];

                for ( int n = 1 ; n < max_num_rdx_nodes_ ; n++ )
                {
                    rdx_.dnodes[n].nnfp = (BNODE *)(&rdx_.dnodes[n+1]);
                    rdx_.dnodes[n].id = 1;
                }
                rdx_.dnodes[max_num_rdx_nodes_].nnfp = NULL;
                rdx_.dnodes[max_num_rdx_nodes_].id = 1;
            }  // initialize()

        public:
            MKRdxPat( int mnrn, int nk, int nkb ) : max_num_rdx_nodes_(mnrn), num_keys_(nk), num_key_bytes_(nkb)
            {
                unsigned char *fptr;
                const size_t MSG_BUF_SIZE = 256;
                char msg[MSG_BUF_SIZE];


                if ( max_num_rdx_nodes_ < 1 )
                {
                    cout << "max_num_rdx_nodes_ = " << max_num_rdx_nodes_ << "(must be > 0)" << endl;
                    throw "max_num_rdx_nodes_ < 1(must be > 0)";
                }

                if ( num_keys_ < 1 )
                {
                    cout << "num_keys_ = " << num_keys_ << "(must be > 0)" << endl;
                    throw "num_keys_ < 1(must be > 0)";
                }

                if ( num_key_bytes_ < 1 )
                {
                    cout << "num_key_bytes_ = " << num_key_bytes_ << "(must be > 0)" << endl;
                    throw "num_key_bytes_ < 1(must be > 0)";
                }

                // total calloc() segments =  5(in PNODE_) + ( max_num_rdx_nodes_+1 ) * 3(in DNODE)
                size_t calloc_size =
                                     (num_keys_) * sizeof(BNODE *) +                         // BNODE *head[num_keys_]
                                     (max_num_rdx_nodes_+1) * sizeof(app_data *) +           // app_data *app_data_ptrs[max_num_rdx_nodes_+1]
                                     ((max_num_rdx_nodes_+1) * num_keys_) * sizeof(BNODE) +  // BNODE bnodes[max_num_rdx_nodes_+1][num_keys_]
                                     (num_keys_) * sizeof(BNODE *) +                         // BNODE *bfree_head[num_keys_]
                                     (max_num_rdx_nodes_+1) * sizeof(DNODE);                 // DNODE dnodes[max_num_rdx_nodes_+1]

                for ( int i = 0 ; i < max_num_rdx_nodes_+1 ; i++ )
                {
                    calloc_size +=
                                  (num_keys_) * sizeof(unsigned int) +                       // unsigned int br[num_keys_]
                                  (num_keys_) * sizeof(BNODE *) +                            // BNODE *p[num_keys_]
                                  (num_keys_ * (1+num_key_bytes_)) * sizeof(unsigned char);  // unsigned char key[num_keys_][1+num_key_bytes_]
                }

                fptr = (unsigned char *)calloc( calloc_size, sizeof(unsigned char) );

                // dynamic allocation for PNODE_
                rdx_.head = (BNODE **)(fptr);
                fptr += (num_keys_) * sizeof(BNODE *);
                snprintf(msg, MSG_BUF_SIZE, "PNODE_(%d,%d,%d): MKRdxPat(): calloc(): BNODE *head[num_keys_]",
                    max_num_rdx_nodes_, num_keys_, num_key_bytes_);
                free_msg_.push_back(msg);

                rdx_.app_data_ptrs = (app_data **)(fptr);
                fptr += (max_num_rdx_nodes_+1) * sizeof(DNODE *);
                snprintf(msg, MSG_BUF_SIZE, "PNODE_(%d,%d,%d): MKRdxPat(): calloc(): DNODE *node_ptrs[max_num_rdx_nodes_+1]",
                    max_num_rdx_nodes_, num_keys_, num_key_bytes_);
                free_msg_.push_back(msg);

                rdx_.bnodes = (BNODE *)(fptr);
                fptr += ((max_num_rdx_nodes_+1) * num_keys_) * sizeof(BNODE);
                snprintf(msg, MSG_BUF_SIZE, "PNODE_(%d,%d,%d): MKRdxPat(): calloc(): BNODE bnodes[max_num_rdx_nodes_+1][num_keys_]",
                    max_num_rdx_nodes_, num_keys_, num_key_bytes_);
                free_msg_.push_back(msg);

                rdx_.bfree_head = (BNODE **)(fptr);
                fptr += (num_keys_) * sizeof(BNODE *);
                snprintf(msg, MSG_BUF_SIZE, "PNODE_(%d,%d,%d): MKRdxPat(): calloc(): BNODE *bfree_head[num_keys_]",
                    max_num_rdx_nodes_, num_keys_, num_key_bytes_);
                free_msg_.push_back(msg);

                rdx_.dnodes = (DNODE *)(fptr);
                fptr += (max_num_rdx_nodes_+1) * sizeof(DNODE);
                snprintf(msg, MSG_BUF_SIZE, "PNODE_(%d,%d,%d): MKRdxPat(): calloc(): DNODE dnodes[max_num_rdx_nodes_+1]",
                    max_num_rdx_nodes_, num_keys_, num_key_bytes_);
                free_msg_.push_back(msg);


                // dynamic allocation for DNODE
                for ( int i = 0 ; i < max_num_rdx_nodes_+1 ; i++ )
                {
                    snprintf(msg, MSG_BUF_SIZE, "DNODE(%d): MKRdxPat(): calloc(): br[num_keys_], p[num_keys_], key[num_keys_][1+num_key_bytes_]", i);
                    free_msg_.push_back(msg);

                    rdx_.dnodes[i].br = (unsigned int *) fptr;
                    fptr += (num_keys_) * sizeof(unsigned int);

                    rdx_.dnodes[i].p = (BNODE **)(fptr);
                    fptr += (num_keys_) * sizeof(BNODE *);

                    rdx_.dnodes[i].key = (unsigned char *) fptr;
                    fptr += (num_keys_ * (1+num_key_bytes_)) * sizeof(unsigned char);
                }

                initialize();
            }  // MKRdxPat()


            ~MKRdxPat()
            {
                size_t size;

                size = free_ptr_.size();

                for ( size_t i = 0 ; i < size ; i++ )
                {
                    free( (void *)(free_ptr_[i]) );
                }

                #ifdef DEBUG
                size = free_msg_.size();

                for ( int i = 0 ; i < size ; i++ )
                {
                    cout << "DEBUG: " << free_msg_[i] << endl;
                }
                #endif
            }  // ~MKRdxPat()

            /*
             *======================================================================================================================
             *     insert()
             *
             * Purpose: 
             *     remove a data node from the free list in the PNODE_ data structure and insert this node into the trie with keys
             *     key[NUM_KEYS][1+NUM_KEY_BYTES].  the second subscript first byte for all keys(key[k][0]) is the key boolean.
             *     the key boolean should always be set to 1 since all keys are needed for insertion.  the actual key bytes
             *     (key[k][1 to NUM_KEY_BYTES]) follow the key boolean.  each key must be unique within it's key
             *     index(0 to NUM_KEYS-1).
             *
             * Usage:
             *     struct app_data
             *     {
             *         int data;
             *     }
             *
             *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES];
             *     app_data *app_datap;
             *     int return_code;
             *
             *     return_code = rdx->insert((unsigned char *)key, &app_datap);
             *
             * Returns:
             *     1. if insertion is successful then set return_code to 0 and set app_datap to point to app_data
             *     2. if any key is found to already exist then set return_code to 1 and set app_datap to point to app_data
             *     3. if no data nodes are on the free list then set return_code to 2 and set app_datap to NULL
             *     4. if any key boolean is not 1 set return_code to 3 and set app_datap to NULL
             *
             * Parameters:
             *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES] - NUM_KEYS keys - one byte key boolean and NUM_KEY_BYTES key bytes
             *     app_data **app_datapp                        - pointer to pointer to the inserted data node app_data struct
             *                                                    or NULL if insertion fails
             *
             * Comments:
             */

                int
            insert
                (
                    unsigned char *key,  // unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
                    app_data **app_datapp
                )
            {
                // during search p[] and c[] are successively set to parent and child nodes down the trie.
                // if a new key is to be inserted bna will be the branch node address linked between p[] and c[]
                // and dna will be the data node address linked as the other child of bna.  num_keys_ branch nodes
                // are always added with one data node.  lr[k] is used to set the br indicator in all nodes - lr[k]=1:
                // node is a right link of its parent, lr[k]=0: node is a left link of its parent.  key_bit is the
                // leftmost bit that key is different from terminating key actually found in trie.
                static BNODE **p, **bna;  // static BNODE *p[num_keys_], *bna[num_keys_];
                DNODE *dna;
                static unsigned int *lr;  // static unsigned int lr[num_keys_];
                static unsigned int *key_bit;  // static unsigned int key_bit[num_keys_];

                // c[] is declared as BNODE * but is cast to DNODE * at the end of a search.
                // this is because all searches go through a series of BNODES and end at a DNODE.
                // the determination of search termination is made by examining c[k]->id == 1,
                // which indicates a data node.  since c[k] is declared a BNODE * and the id
                // field is in both node types, then the id field must be at the start of the
                // typedef and in the same relative position in order to get the correct value.
                static BNODE **c;  // static BNODE *c[num_keys_];

                static unsigned char *ky;  // static unsigned char ky[num_keys_][1+num_key_bytes_];

                static bool alloc = false;


                if ( alloc == false )
                {
                    void *fptr;

                    alloc = true;

                    fptr = calloc( num_keys_, sizeof(BNODE *) );
                    p = (BNODE **)(fptr);
                    free_ptr_.push_back(fptr);
                    free_msg_.push_back("insert(): calloc(): static BNODE *p[num_keys_]");

                    fptr = calloc( num_keys_, sizeof(BNODE *) );
                    bna = (BNODE **)(fptr);
                    free_ptr_.push_back(fptr);
                    free_msg_.push_back("insert(): calloc(): static BNODE *bna[num_keys_]");

                    fptr = calloc( num_keys_, sizeof(unsigned int) );
                    lr = (unsigned int *)(fptr);
                    free_ptr_.push_back(fptr);
                    free_msg_.push_back("insert(): calloc(): static unsigned int lr[num_keys_]");

                    fptr = calloc( num_keys_, sizeof(unsigned int) );
                    key_bit = (unsigned int *)(fptr);
                    free_ptr_.push_back(fptr);
                    free_msg_.push_back("insert(): calloc(): static unsigned int key_bit[num_keys_]");

                    fptr = calloc( num_keys_, sizeof(BNODE *) );
                    c = (BNODE **)(fptr);
                    free_ptr_.push_back(fptr);
                    free_msg_.push_back("insert(): calloc(): static BNODE *c[num_keys_]");

                    fptr = calloc( num_keys_ * (1+num_key_bytes_), sizeof(unsigned char));
                    ky = (unsigned char *)(fptr);
                    free_ptr_.push_back(fptr);
                    free_msg_.push_back("insert(): calloc(): static unsigned char ky[num_keys_][1+num_key_bytes_]");
                }

                for ( int k = 0 ; k < num_keys_ ; k++ )
                {
                    // for insertion all key booleans must be 1
                    if ( key[k*(1+num_key_bytes_)+0] != 1 )
                    {
                        *app_datapp = NULL;
                        return 3;
                    }
                }

                for ( int k = 0 ; k < num_keys_ ; k++ )
                {
                    // copy key to storage with extra byte for comparison
                    ky[k*(1+num_key_bytes_)+0] = 0;
                    memmove( &ky[k*(1+num_key_bytes_)+1], &key[k*(1+num_key_bytes_)+1], num_key_bytes_ );

                    // search for key in rdx trie
                    c[k] = (BNODE *)(rdx_.head[k]->l);
                    while ( c[k]->id == 0 )
                    {
                        c[k] = ( gbit( &ky[k*(1+num_key_bytes_)], c[k]->b ) ) ? (BNODE *)(c[k]->r) : (BNODE *)(c[k]->l);
                    }

                    // if key found return node in arg and set return to 1
                    if ( memcmp( &ky[k*(1+num_key_bytes_)], &((DNODE *)c[k])->key[k*(1+num_key_bytes_)], num_key_bytes_+1 ) == 0 )
                    {
                        *app_datapp = &( ((DNODE *)(c[k]))->data );
                        return 1;
                    }
                }

                for ( int k = 0 ; k < num_keys_ ; k++ )
                {
                    // if no nodes free set app_datapp to NULL and return 2
                    if ( rdx_.bfree_head[k] == NULL || rdx_.dfree_head == NULL )
                    {
                        *app_datapp = NULL;
                        return 2;
                    }
                }

                dna = rdx_.dfree_head;
                rdx_.dfree_head = (DNODE *)(rdx_.dfree_head->nnfp);
                rdx_.tot_nodes++;

                for ( int k = 0 ; k < num_keys_ ; k++ )
                {
                    // key not found in rdx trie: find leftmost bit that key is different from terminating
                    // key actually found in trie.
                    //
                    // NOTE: the extra high order byte should insure that any time we arrive at this code
                    //       ((DNODE *)c[k])->key[k] and key are different.  the maximum key bit will be the
                    //       first bit of the extra byte.  if the keys are not different before or at bit zero
                    //       (the furthest right most bit) then this statement will call gbit() with a negative
                    //       value of key_bit and a memory violation will probably result or at the very least
                    //       wholly incorrect insertion.
                    key_bit[k] = num_key_bytes_*8;
                    while ( gbit( &((DNODE *)c[k])->key[k*(1+num_key_bytes_)], key_bit[k] ) == gbit( &ky[k*(1+num_key_bytes_)+0], key_bit[k] ) )
                    {
                        key_bit[k]--;
                    }

                    // allocate new nodes from free list
                    bna[k] = rdx_.bfree_head[k];
                    rdx_.bfree_head[k] = (BNODE *)(rdx_.bfree_head[k]->p);

                    // search for location to insert new key - c[k]->id != 0 new node will be at external trie
                    // node insertion, c[k]->b <= key_bit new node will be internal trie node insertion
                    lr[k] = 0;
                    p[k] = rdx_.head[k];
                    c[k] = (BNODE *)(rdx_.head[k]->l);
                    while ( c[k]->id == 0 && c[k]->b > key_bit[k] )
                    {
                        p[k] = c[k];
                        if ( gbit( &ky[k*(1+num_key_bytes_)+0], c[k]->b ) )
                        {
                            c[k] = (BNODE *)(c[k]->r);
                            lr[k] = 1;
                        }
                        else
                        {
                            c[k] = (BNODE *)(c[k]->l);
                            lr[k] = 0;
                        }
                    }

                    // set parent and child links to new branch node address
                    if ( lr[k] == 0 )
                    {
                        p[k]->l = bna[k];
                    }
                    else
                    {
                        p[k]->r = bna[k];
                    }

                    if ( c[k]->id == 0 )
                    {
                        ((BNODE *)c[k])->p = bna[k];
                    }
                    else
                    {
                        ((DNODE *)c[k])->p[k] = bna[k];
                    }

                    // set new branch node links and both child parent branches(dna->br[k],c[k]->br)
                    bna[k]->br = lr[k];
                    bna[k]->b = key_bit[k];
                    bna[k]->p = p[k];
                    if ( gbit( &ky[k*(1+num_key_bytes_)+0], bna[k]->b ) )
                    {
                        bna[k]->l = (void *)(c[k]);
                        bna[k]->r = (void *)(dna);
                        dna->br[k] = 1;
                        if ( c[k]->id == 0 )
                        {
                            ((BNODE *)c[k])->br = 0;
                        }
                        else
                        {
                            ((DNODE *)c[k])->br[k] = 0;
                        }
                    }
                    else
                    {
                        bna[k]->l = (void *)(dna);
                        bna[k]->r = (void *)(c[k]);
                        dna->br[k] = 0;
                        if ( c[k]->id == 0 )
                        {
                            ((BNODE *)c[k])->br = 1;
                        }
                        else
                        {
                            ((DNODE *)c[k])->br[k] = 1;
                        }
                    }

                    #ifdef DEBUG
                    printf("DEBUG: ky k = %d\n", k);
                    for ( int i = 0 ; i < num_key_bytes_+1 ; i++ )
                    {
                        printf("%X ", *(&ky[k*(1+num_key_bytes_)+0]+i) );
                    }
                    printf("\n");
                    #endif

                    // set child key
                    memmove( &(dna->key[k*(1+num_key_bytes_)]), &ky[k*(1+num_key_bytes_)], num_key_bytes_+1 );

                    #ifdef DEBUG
                    printf("DEBUG: dna->key[] k = %d\n", k);
                    for ( int i = 0 ; i < num_key_bytes_+1 ; i++ )
                    {
                        printf("%X ", *( &(dna->key[k*(1+num_key_bytes_)])+i ) );
                    }
                    printf("\n\n");
                    #endif

                    // set back pointer
                    dna->p[k] = bna[k];
                }

                // set node to allocated status
                dna->alloc = 1;

                #ifdef DEBUG
                printf("DEBUG: dna = %p \n", dna );
                #endif

                // new node insertion successful, set return values
                *app_datapp = &( dna->data );

                #ifdef DEBUG
                cout << "DEBUG: end insert(): " << dna->data.id << endl;
                #endif

                return 0;
            }  // insert()

            /*
             *======================================================================================================================
             *     search()
             *
             * Purpose: 
             *     search trie for the data node with the keys key[NUM_KEYS][1+NUM_KEY_BYTES].  the second subscript first byte
             *     for all keys(key[k][0]) is the key boolean.  the key boolean should be set to 1(use the key) or
             *     0(do not use key).  the actual key bytes(key[k][1 to num_key_bytes_]) follow the key boolean.  thus, 1 to
             *     num_keys_ keys may be used in any given search.  only one key is necessary to find the data node.  each key
             *     must be unique within it's key index(0 - NUM_KEYS-1).
             *
             * Usage:
             *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES];
             *     app_data *app_datap;
             *
             *     app_datap = rdx->search((unsigned char *)key);
             *
             * Returns:
             *     1. if search is successful then return pointer to app_data(app_datap)
             *     2. if any key boolean is not 0 or 1 return NULL
             *     3. if two key searches end at different data nodes return NULL
             *     4. if any key search does not end at a data node return NULL
             *     5. if no key boolean is 1 then no keys are used and return NULL
             *    
             * Parameters:
             *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES] - NUM_KEYS keys - one byte key boolean and NUM_KEY_BYTES key bytes
             *
             * Comments:
             *     e.g. set NUM_KEYS=3 and NUM_KEY_BYTES=4
             *
             *          example 1:
             *          in key[][]:
             *             key number  key
             *             0           01 aa bb cc dd    
             *             1           01 ee ee ee ee
             *             2           01 ff ff ff ff
             *
             *             use all three keys(0,1,2) in the search
             *
             *          example 2:
             *          in key[][]:
             *             0           00 aa bb cc dd
             *             1           01 ee ee ee ee
             *             2           00 ff ff ff ff
             *
             *             use only key 1 in the search
             *
             *          example 3:
             *          in key[][]:
             *             0           01 00 00 00 01
             *             1           01 00 00 00 01
             *             2           01 00 00 00 01
             *
             *             inserted successfuly - a. all key booleans 1  b. each key unique in it's own key index(0 - 2)
             */

                app_data *
            search
                (
                    unsigned char *key  // unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
                )
            {
                // c[] is declared as BNODE * but is cast to DNODE * at the end of a search.
                // this is because all searches go through a series of BNODES and end at a DNODE.
                // the determination of search termination is made by examining c[k]->id == 1,
                // which indicates a data node.  since c[k] is declared a BNODE * and the id
                // field is in both node types, then the id field must be at the start of the
                // typedef and in the same relative position in order to get the correct value.
                static BNODE *c;
                BNODE *csav;

                static unsigned char *ky;  // static unsigned char ky[num_keys_][1+num_key_bytes_];

                static bool alloc = false;  // static - only first time search() for calloc() calls

                bool firsttime = true;  // not static - every search() call


                if ( alloc == false )
                {
                    void *fptr;

                    alloc = true;


                    fptr = calloc( num_keys_, sizeof(BNODE *) );
                    c = (BNODE *)(fptr);
                    free_ptr_.push_back(fptr);
                    free_msg_.push_back("search(): calloc(): static BNODE *c[num_keys_]");

                    fptr = calloc( num_keys_ * (1+num_key_bytes_), sizeof(unsigned char));
                    ky = (unsigned char *) fptr;
                    free_ptr_.push_back(fptr);
                    free_msg_.push_back("search(): calloc(): static unsigned char ky[num_keys_][1+num_key_bytes_]");
                }

                int n = 0;
                for ( int k = 0 ; k < num_keys_ ; k++ )
                {
                    #ifdef DEBUG
                    cout << "DEBUG: k = " << k << endl;
                    #endif

                    if ( key[k*(1+num_key_bytes_)+0] == 0 )
                    {
                        continue;  // if key boolean is 0 skip key
                    }
                    else if ( key[k*(1+num_key_bytes_)+0] == 1 )
                    {
                        n++;  // if key boolean is 1 use key
                    }
                    else
                    {
                        return NULL;  // if a key boolean is not 0 or 1
                    }

                    // copy key to storage with extra byte for comparison
                    ky[k*(1+num_key_bytes_)+0] = 0;
                    memmove( &ky[k*(1+num_key_bytes_)+1], &key[k*(1+num_key_bytes_)+1], num_key_bytes_ );

                    // search for key in rdx trie
                    c = (BNODE *)(rdx_.head[k]->l);
                    while ( c->id == 0 )
                    {
                        c = ( gbit( &ky[k*(1+num_key_bytes_)+0], c->b ) ) ? (BNODE *)(c->r) : (BNODE *)(c->l);
                    }

                    #ifdef DEBUG
                    printf("DEBUG: c = %p \n", c );
                    printf("DEBUG: (DNODE *)c = %p \n", (DNODE *)c );
                    printf("DEBUG: ((DNODE *)c)->key = %p \n", ((DNODE *)c)->key );
                    #endif

                    // check if all keys end at the same data node - if not return NULL
                    if ( firsttime == true )
                    {
                        csav = c;
                        firsttime = false;
                    }
                    else
                    {
                        if ( c != csav )
                        {
                            return NULL;  // a key does not end at the same data node as a previous key
                        }
                    }

                    #ifdef DEBUG
                    for ( int i = 0 ; i < num_key_bytes_+1 ; i++ )
                    {
                        printf("%X ", *(&ky[k*(1+num_key_bytes_)+0]+i) );
                    }
                    printf("\n");

                    for ( int i = 0 ; i < num_key_bytes_+1 ; i++ )
                    {
                        printf("DEBUG: %p\n",  &((DNODE *)c)->key[k*(1+num_key_bytes_)]+i );
                        printf("DEBUG: %X\n", *( &((DNODE *)c)->key[k*(1+num_key_bytes_)]+i ) );
                    }
                    printf("\n");

                    printf("DEBUG: dna = %p\n", (DNODE *)c );
                    #endif

                    // if key not found return NULL
                    if ( memcmp( &ky[k*(1+num_key_bytes_)+0], &((DNODE *)c)->key[k*(1+num_key_bytes_)], num_key_bytes_+1 ) != 0 )
                    {
                        #ifdef DEBUG
                        cout << "DEBUG: k = " << "exit2" << endl;
                        #endif

                        return NULL;  // key not found
                    }
                }

                if ( n == 0 )
                {
                    #ifdef DEBUG
                    cout << "DEBUG: " << "k = " << "exit3" << endl;
                    #endif

                    return NULL;  // no keys are used(all key booleans are 0)
                }

                // success - all keys found in the same data node - return data node pointer
                return &( ((DNODE *)(c))->data );
            }  // search()

            /*
             *======================================================================================================================
             *     remove()
             *
             * Purpose: 
             *     remove trie data node with the keys key[NUM_KEYS][1+NUM_KEY_BYTES].  the second subscript first byte for all
             *     keys(key[k][0]) is the key boolean.  the key boolean should be set to 1(use the key) or 0(do not use key).
             *     the actual key bytes(key[k][1 to NUM_KEY_BYTES]) follow the key boolean.  thus, 1 to NUM_KEYS keys may be used
             *     for any given remove.  only one key is necessary to remove the data node.  each key must be unique within it's
             *     key index(0 - NUM_KEYS-1).
             *
             * Usage:
             *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES];
             *     app_data *app_datap;
             *
             *     app_datap = rdx->remove((unsigned char *)key);
             *
             * Returns:
             *     1. if search is successful then return pointer to app_data(app_datap)
             *     2. if any key boolean is not 0 or 1 return NULL
             *     3. if two key searches end at different data nodes return NULL
             *     4. if any key search does not end at a data node return NULL
             *     5. if no key boolean is 1 then no keys are used and return NULL
             *
             * Parameters:
             *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES] - NUM_KEYS keys - one byte key boolean and NUM_KEY_BYTES key bytes
             *
             * Comments:
             *     e.g. set NUM_KEYS=3 and NUM_KEY_BYTES=4
             *
             *          example 1:
             *          in key[][]:
             *             key number  key
             *             0           01 aa bb cc dd    
             *             1           01 ee ee ee ee
             *             2           01 ff ff ff ff
             *
             *             use all three keys(0,1,2) in the search
             *
             *          example 2:
             *          in key[][]:
             *             0           00 aa bb cc dd
             *             1           01 ee ee ee ee
             *             2           00 ff ff ff ff
             *
             *             use only key 1 in the search
             */

                app_data *
            remove
                (
                    unsigned char *key  // unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
                )
            {
                int n;

                // other child pointer
                BNODE *oc;

                // c[] is declared as BNODE * but is cast to DNODE * at the end of a search.
                // this is because all searches go through a series of BNODES and end at a DNODE.
                // the determination of search termination is made by examining c[k]->id == 1,
                // which indicates a data node.  since c[k] is declared a BNODE * and the id
                // field is in both node types, then the id field must be at the start of the
                // typedef and in the same relative position in order to get the correct value.
                static BNODE *c;
                BNODE *csav;

                static unsigned char *ky;  // static unsigned char ky[num_keys_][1+num_key_bytes_];

                static bool alloc = false;  // static - only first time remove() for calloc() calls

                bool firsttime = true;  // not static - every remove() call


                if ( alloc == false )
                {
                    void *fptr;

                    alloc = true;


                    fptr = calloc( num_keys_, sizeof(BNODE *) );
                    c = (BNODE *)(fptr);
                    free_ptr_.push_back(fptr);
                    free_msg_.push_back("remove(): calloc(): static BNODE *c[num_keys_]");

                    fptr = calloc( num_keys_ * (1+num_key_bytes_), sizeof(unsigned char));
                    ky = (unsigned char *) fptr;
                    free_ptr_.push_back(fptr);
                    free_msg_.push_back("remove(): calloc(): static unsigned char ky[num_keys_][1+num_key_bytes_]");
                }

                n = 0;
                for ( int k = 0 ; k < num_keys_ ; k++ )
                {
                    #ifdef DEBUG
                    printf("DEBUG: key boolean = %X\n", key[k*(1+num_key_bytes_)+0]);
                    #endif

                    if ( key[k*(1+num_key_bytes_)+0] == 0 )
                    {
                        continue;
                    }
                    else if ( key[k*(1+num_key_bytes_)+0] == 1 )
                    {
                        n++;  // if key boolean is 1 use key
                    }
                    else
                    {
                        #ifdef DEBUG
                        cout << "DEBUG: k = " << "exit0" << endl;
                        #endif

                        return NULL;  // if a key boolean is not 0 or 1
                    }

                    #ifdef DEBUG
                    cout << "DEBUG: k = " << k << endl;
                    cout << "DEBUG: max_num_rdx_nodes_ = " << max_num_rdx_nodes_ << endl;
                    cout << "DEBUG: num_keys_ = " << num_keys_ << endl;
                    cout << "DEBUG: num_key_bytes_ = " << num_key_bytes_ << endl;
                    #endif


                    // copy key to storage with extra byte for comparison
                    ky[k*(1+num_key_bytes_)+0] = 0;
                    memmove( &ky[k*(1+num_key_bytes_)+1], &key[k*(1+num_key_bytes_)+1], num_key_bytes_ );

                    // search for key in rdx trie
                    c = (BNODE *)(rdx_.head[k]->l);
                    while ( c->id == 0 )
                    {
                        c = ( gbit( &ky[k*(1+num_key_bytes_)+0], c->b ) ) ? (BNODE *)(c->r) : (BNODE *)(c->l);
                    }

                    // check if all keys end at the same data node - if not return NULL
                    if ( firsttime == true )
                    {
                        csav = c;
                        firsttime = false;
                    }
                    else
                    {
                        if ( c != csav )
                        {
                            return NULL;  // a key does not end at the same data node as a previous key
                        }
                    }

                    #ifdef DEBUG
                    for ( int i = 0 ; i < num_key_bytes_+1 ; i++ )
                    {
                        printf("DEBUG: %X ", *(&ky[k*(1+num_key_bytes_)+0]+i) );
                    }
                    printf("\n");

                    for ( int i = 0 ; i < num_key_bytes_+1 ; i++ )
                    {
                        printf("DEBUG: %p\n",  &((DNODE *)c)->key[k*(1+num_key_bytes_)]+i );
                        printf("DEBUG: %X\n", *( &((DNODE *)c)->key[k*(1+num_key_bytes_)]+i ) );
                    }
                    printf("\n");
                    #endif

                    // if key not found return NULL
                    if ( memcmp( &ky[k*(1+num_key_bytes_)+0], &((DNODE *)c)->key[k*(1+num_key_bytes_)], num_key_bytes_+1 ) != 0 )
                    {
                        return NULL;  // key not found
                    }
                }

                if ( n == 0 )
                {
                    #ifdef DEBUG
                    cout << "DEBUG: k = " << "exit3" << endl;
                    #endif

                    return NULL;  // no keys are used(all key booleans are 0)
                }

                // for each key reset pointers for num_keys_ branch nodes and the data node
                for ( int k = 0 ; k < num_keys_ ; k++ )
                {
                    // set ptr to other child of branch node to remove
                    if ( ((DNODE *)c)->br[k] == 0 )
                    {
                        oc = ((BNODE *)((BNODE *)((DNODE *)c)->p[k])->r);
                    }
                    else
                    {
                        oc = ((BNODE *)((BNODE *)((DNODE *)c)->p[k])->l);
                    }

                    if ( oc->id == 0 )
                    {
                        // other child is a branch node - repair other child ptrs
                        ((BNODE *)oc)->p = ((BNODE *)((DNODE *)c)->p[k])->p;
                        ((BNODE *)oc)->br = ((BNODE *)((DNODE *)c)->p[k])->br;
                    }
                    else
                    {
                        // other child is a data node - repair other child ptrs
                        ((DNODE *)oc)->p[k] = ((BNODE*)((BNODE *)((DNODE *)c)->p[k])->p);
                        ((DNODE *)oc)->br[k] = ((BNODE *)((DNODE *)c)->p[k])->br;
                    }

                    // repair ptr of parent of parent
                    if ( ((BNODE *)((DNODE *)c)->p[k])->br == 0 )
                    {
                        ((BNODE *)((BNODE *)((DNODE *)c)->p[k])->p)->l = oc;
                    }
                    else
                    {
                        ((BNODE *)((BNODE *)((DNODE *)c)->p[k])->p)->r = oc;
                    }

                    // return branch nodes to free list
                    ((BNODE *)((DNODE *)c)->p[k])->p = rdx_.bfree_head[k];
                    rdx_.bfree_head[k] = ((DNODE *)c)->p[k];
                }

                // set node to free status
                ((DNODE *)c)->alloc = 0;

                // return data node to free list
                ((DNODE *)c)->nnfp = rdx_.dfree_head;
                rdx_.dfree_head = (DNODE *)c;

                // decrement total allocated nodes
                rdx_.tot_nodes--;

                // set return pointer to removed node app_data
                return &( ((DNODE *)(c))->data );
            }  // remove()

            /*
             *======================================================================================================================
             *     sort()
             *
             * Purpose: 
             *     sort pointers to app_data by key index k in PNODE_ data structure in key ascending order.
             *
             * Usage:
             *     app_data **app_datapp;
             *     int k;
             *     int return_code;
             *
             *     return_code = rdx->sort(&app_datapp, k);
             *
             * Returns:
             *     int return_code - the number of sorted nodes.  if no keys return 0.  if k outside 0 to NUM_KEYS-1 return -1.
             *     app_data **app_datapp - pointer to array of pointers to app_data
             *
             * Parameters:
             *     app_data ***app_data - pointer to pointer to pointer to app_data
             *     int k                - key index(0 - NUM_KEYS-1)
             *
             * Comments:
             *     (app_data *)app_data[0 to return_code-1]     - array of pointers to app_data
             *     (app_data *)app_data[0 to return_code-1]->id - to access app_data data
             */

                int
            sort
                (
                    app_data ***app_datappp,
                    int k
                )
            {
                if ( k < 0 || k > num_keys_-1 )
                {
                    return -1;
                }

                // set full node_ptrs array to 0xf0 - shouldn't affect anything - then 0xf0 pattern
                // will be in array elements not allocated - makes debugging easier
                memset( rdx_.app_data_ptrs, 0xf0, (max_num_rdx_nodes_+1) * sizeof(app_data *) );

                rdx_.app_data_ptrs_cnt = 0;

                recursive( rdx_.head[k] );

                *app_datappp = rdx_.app_data_ptrs;

                return rdx_.app_data_ptrs_cnt-1;
            }  // sort()

            /*
             *======================================================================================================================
             *     nodes()
             *
             * Purpose: 
             *     return the number of allocated data nodes in the trie.
             *
             * Usage:
             *     int n;
             *
             *     n = rdx->nodes();
             *
             * Returns:
             *     int n - number of allocated nodes in the trie(0 - NAX_NUM_RDX_NODES)
             *
             * Parameters:
             *     None
             *
             * Comments:
             *     the number of actual nodes in the trie is MAX_NUM_RDX_NODES+1.  the extra node is the always allocated impossible
             *     key node with key(s) 0xff.  the number of user available nodes is MAX_NUM_RDX_NODES.  this function returns
             *     only the number of user allocated nodes in the trie.
             */

                int
            nodes
                (
                )
            {
               return rdx_.tot_nodes;
            }  // nodes()

            /*
             *======================================================================================================================
             *     print()
             *
             * Purpose: 
             *     Used in two modes:
             *         a. the first arg is NULL
             *
             *            print all the data in branch and data nodes for the entire trie.  for each data node allocated there
             *            will be NUM_KEYS new branch nodes allocated.  thus, for n data nodes allocated, there will be n*NUM_KEYS
             *            branch nodes printed and n data nodes printed.  the path along branch nodes to data nodes is not
             *            demonstrated.  useful only for small tries for debugging purposes.
             *
             *         b. the first arg is an array of 'unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]' of an existing allocated
             *            data node
             *
             *            print the data node and each branch node for each key(1 - NUM_KEYS) that leads to the found data node.
             *            the application data is not printed, only the trie structure data.
             *
             *     Setting 'unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]':
             *
             *     the second subscript first byte for all keys(key[k][0]) is the key boolean.  the key boolean should be set to
             *     1(use the key) or 0(do not use key).  the actual key bytes(key[k][1 to NUM_KEY_BYTES]) follow the key boolean.
             *     thus, 1 to NUM_KEYS keys may be used.  only one key is necessary to find the data node.  each key must be unique
             *     within it's key index(0 - NUM_KEYS-1).
             *
             * Usage:
             *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES];
             *     ofstream& os;
             *     int return_code;
             *
             *     return_code = rdx->print(NULL, os);
             *     return_code = rdx->print((unsigned char *)key, os);
             *
             * Returns:
             *     1. return_code = 0 on success
             *     2. return_code = 1 if data node for key[][] not found
             *
             * Parameters:
             *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES] - NUM_KEYS keys - one byte key boolean and NUM_KEY_BYTES key bytes
             *     ofstream& os                                 - output stream
             *
             * Comments:
             *     e.g. set NUM_KEYS=3 and NUM_KEY_BYTES=4
             *
             *          example 1:
             *          in key[][]:
             *             key number  key
             *             0           01 aa bb cc dd    
             *             1           01 ee ee ee ee
             *             2           01 ff ff ff ff
             *
             *             use all three keys(0,1,2) in the search
             *
             *          example 2:
             *          in key[][]:
             *             0           00 aa bb cc dd
             *             1           01 ee ee ee ee
             *             2           00 ff ff ff ff
             *
             *             use only key 1 in the search
             */

                int
            print
                (
                    unsigned char *key,  // unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
                    ofstream& os
                )
            {
                const size_t MSG_BUF_SIZE = 256;
                char msg[MSG_BUF_SIZE];

                static unsigned char *ky;  // unsigned char ky[1+num_key_bytes_];

                BNODE *c;
                app_data *app_datap;

                static bool alloc = false;


                if ( alloc == false )
                {
                    void *fptr;

                    alloc = true;

                    fptr = calloc( 1+num_key_bytes_, sizeof(unsigned char));
                    ky = (unsigned char *) fptr;
                    free_ptr_.push_back(fptr);
                    free_msg_.push_back("print(): calloc(): unsigned char ky[1+num_key_bytes_]");
                }

                if ( key == NULL )
                {
                    int tot_nodes;

                    os << "==========\n\n";
                    os << "print(NULL, os) called with NULL first arg.\n\n";
                    os << "print all the data in branch and data nodes for the entire trie.  for each data node allocated there\n"
                          "will be num_keys_ new branch nodes allocated.  thus, for n data nodes allocated, there will be n*num_keys_\n"
                          "branch nodes printed and n data nodes printed.  the path along branch nodes to data nodes is not\n"
                          "demonstrated.  useful only for small tries for debugging purposes.\n\n";

                    tot_nodes = nodes();

                    os << "Total nodes allocated: " << tot_nodes << "  Number of keys: " << num_keys_ << "  Key length: " << num_key_bytes_ << "\n\n\n";
                    os << "NODE n\n\n";
                    os << "Branch Node Format:\n";
                    os << "  key = (key index into key array in data node)\n";
                    os << "  nsn = (node sequence number, 0-max_num_rdx_nodes_+1)\n";
                    os << "   id = (node id - 0 = branch node, 1 - data node)\n";
                    os << "  bna = (branch node address)\n";
                    os << "   br = (left/right branch of parent - 0 = left, 1 = right)\n";
                    os << "    p = (parent address)\n";
                    os << "    b = (PATRICIA key bit(0 on right) at which this branch node distinguishes keys)\n";
                    os << "    l = (address of left node child)\n";
                    os << "    r = (address of right node child)\n";
                    os << endl;
                    os << "Data Node Format:\n";
                    os << "   id = (node id - 0 = branch node, 1 - data node)\n";
                    os << "  dna = (data node address)\n";
                    os << "   br = (left/right branch of parent - 0 = left, 1 = right)\n";
                    os << "    p = (parent address)\n";
                    os << "  nsn = (node sequence number, 0-max_num_rdx_nodes_+1)\n";
                    os << "alloc = (0-node not allocated, 1-node allocated)\n";
                    os << "key n = (hex key value)\n";
                    os << "      .\n";
                    os << "      .\n";
                    os << "      .\n\n\n";

                    for ( int n = 0 ; n < max_num_rdx_nodes_+1 ; n++ )
                    {
                        if ( rdx_.dnodes[n].alloc != 1 )
                        {
                            continue;
                        }

                        os << "NODE " << n << "\n\n";

                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%7s %18d  ", "  key =", k);
                            os << msg;
                        }
                        os << endl;

                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%7s %18d  ", "  nsn =", rdx_.bnodes[n*num_keys_+k].nsn);
                            os << msg;
                        }
                        os << endl;

                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%7s %18d  ", "   id =", rdx_.bnodes[n*num_keys_+k].id);
                            os << msg;
                        }
                        os << endl;

                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%7s %18p  ", "  bna =", (void *)&rdx_.bnodes[n*num_keys_+k]);
                            os << msg;
                        }
                        os << endl;

                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%7s %18d  ", "   br =", rdx_.bnodes[n*num_keys_+k].br);
                            os << msg;
                        }
                        os << endl;

                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%7s %18p  ", "    p =", (void *)rdx_.bnodes[n*num_keys_+k].p);
                            os << msg;
                        }
                        os << endl;

                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%7s %18d  ", "    b =", rdx_.bnodes[n*num_keys_+k].b);
                            os << msg;
                        }
                        os << endl;

                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%7s %18p  ", "    l =", (void *)rdx_.bnodes[n*num_keys_+k].l);
                            os << msg;
                        }
                        os << endl;

                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%7s %18p  ", "    r =", (void *)rdx_.bnodes[n*num_keys_+k].r);
                            os << msg;
                        }
                        os << "\n\n";

                        snprintf(msg, MSG_BUF_SIZE, "%7s %18d  ", "   id =", rdx_.dnodes[n].id);
                        os << msg;
                        os << endl;
                        snprintf(msg, MSG_BUF_SIZE, "%7s %18p  ", "  dna =", (void *)&rdx_.dnodes[n]);
                        os << msg;
                        os << endl;
                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%7s %18d  ", "   br =", rdx_.dnodes[n].br[k]);
                            os << msg;
                        }
                        os << endl;

                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%7s %18p  ", "    p =", (void *)rdx_.dnodes[n].p[k]);
                            os << msg;
                        }
                        os << endl;

                        snprintf(msg, MSG_BUF_SIZE, "%7s %18d  ", "  nsn =", rdx_.dnodes[n].nsn);
                        os << msg;
                        os << endl;
                        snprintf(msg, MSG_BUF_SIZE, "%7s %18d  ", "alloc =", rdx_.dnodes[n].alloc);
                        os << msg;
                        os << endl;

                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "  key %2d = ", k);
                            os << msg;
                            for ( int b = 0 ; b < num_key_bytes_ ; b++ )
                            {
                                snprintf(msg, MSG_BUF_SIZE, "%02x ", rdx_.dnodes[n].key[k*(1+num_key_bytes_)+b+1]);
                                os << msg;
                            }
                            os << endl;
                        }

                        os << endl;
                    }

                    return 0;
                }
                else
                {
                    os << "==========\n\n";
                    os << "print(key, os) called with key[][] first arg.\n\n";
                    os << "print the data node and each branch node for each key(1-num_keys_) that leads to the found data node.\n"
                                "the application data is not printed, only the trie structure data.  the number of branch nodes printed\n"
                                "is indeterminate; it depends on the order of the data node insertion.  as few as one branch node and\n"
                                "as many as n branch nodes(n being the total number of data nodes already inserted) could be printed.\n\n";

                    os << "Search for data node with key(s)(includes key boolean):\n";
                    for ( int k = 0 ; k < num_keys_ ; k++ )
                    {
                        snprintf(msg, MSG_BUF_SIZE, "  key %2d = ", k);
                        os << msg;
                        for ( int b = 0 ; b < 1+num_key_bytes_ ; b++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%02x ", key[k*(1+num_key_bytes_)+b]);
                            os << msg;
                        }
                        os << endl;
                    }
                    os << endl;

                    os << "Branch Node Format:\n";
                    os << "  nsn = (node sequence number, 0-max_num_rdx_nodes_+1)\n";
                    os << "   id = (node id - 0 = branch node, 1 - data node)\n";
                    os << "  bna = (branch node address)\n";
                    os << "   br = (left/right branch of parent - 0 = left, 1 = right)\n";
                    os << "    p = (parent address)\n";
                    os << "    b = (PATRICIA key bit(0 on right) at which this branch node distinguishes keys)\n";
                    os << "    l = (address of left node child)\n";
                    os << "    r = (address of right node child)\n";
                    os << endl;
                    os << "Data Node Format:\n";
                    os << "   id = (node id - 0 = branch node, 1 - data node)\n";
                    os << "  dna = (data node address)\n";
                    os << "   br = (left/right branch of parent - 0 = left, 1 = right)\n";
                    os << "    p = (parent address)\n";
                    os << "  nsn = (node sequence number, 0-max_num_rdx_nodes_+1)\n";
                    os << "alloc = (0-node not allocated, 1-node allocated)\n";
                    os << "key n = (hex key value)\n";
                    os << "      .\n";
                    os << "      .\n";
                    os << "      .\n\n\n";

                    if ( (app_datap = search(key)) == NULL )
                    {
                        snprintf(msg, MSG_BUF_SIZE, "print():(file %s  line %d): Key not found.\n", __FILE__, __LINE__);
                        os << msg;
                        return 1;
                    }

                    for ( int k = 0 ; k < num_keys_ ; k++ )
                    {
                        // copy key to storage with extra byte for comparison
                        ky[k*(1+num_key_bytes_)+0] = 0;
                        memmove( &ky[k*(1+num_key_bytes_)+1], &key[k*(1+num_key_bytes_)+1], num_key_bytes_ );

                        os << "==========\n";
                        snprintf(msg, MSG_BUF_SIZE, "  key %2d = ", k);
                        os << msg;
                        for ( int b = 0 ; b < 1+num_key_bytes_ ; b++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%02x ", key[k*(1+num_key_bytes_)+b]);
                            os << msg;
                        }
                        os << "\n\n";

                        c = (BNODE *)rdx_.head[k];
                        while ( c->id == 0 )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "  nsn = %d\n", c->nsn);
                            os << msg;
                            snprintf(msg, MSG_BUF_SIZE, "   id = %d\n", c->id);
                            os << msg;
                            snprintf(msg, MSG_BUF_SIZE, "  bna = %p\n", (void *)c);
                            os << msg;
                            snprintf(msg, MSG_BUF_SIZE, "   br = %d\n", c->br);
                            os << msg;
                            snprintf(msg, MSG_BUF_SIZE, "    p = %p\n", (void *)c->p);
                            os << msg;
                            snprintf(msg, MSG_BUF_SIZE, "    b = %-3d\n", c->b);
                            os << msg;
                            snprintf(msg, MSG_BUF_SIZE, "    l = %p\n", (void *)c->l);
                            os << msg;
                            snprintf(msg, MSG_BUF_SIZE, "    r = %p\n", (void *)c->r);
                            os << msg;
                            os << endl;

                            c = ( gbit( ky, c->b ) ) ? (BNODE *)c->r : (BNODE *)c->l;
                        }

                        snprintf(msg, MSG_BUF_SIZE, "   id = %d\n", ((DNODE *)c)->id);
                        os << msg;
                        snprintf(msg, MSG_BUF_SIZE, "%7s %18p\n", "  dna =", (void *)c);
                        os << msg;
                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%7s %18d  ", "   br =", ((DNODE *)c)->br[k]);
                            os << msg;
                        }
                        os << endl;

                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "%7s %18p  ", "    p =", (void *)((DNODE *)c)->p[k]);
                            os << msg;
                        }
                        os << endl;

                        snprintf(msg, MSG_BUF_SIZE, "  nsn = %d\n", ((DNODE *)c)->nsn);
                        os << msg;
                        snprintf(msg, MSG_BUF_SIZE, "alloc = %d\n", ((DNODE *)c)->alloc);
                        os << msg;
                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "  key %2d = ", k);
                            os << msg;
                            for ( int b = 0 ; b < 1+num_key_bytes_ ; b++ )
                            {
                                snprintf(msg, MSG_BUF_SIZE, "%02x ", key[k*(1+num_key_bytes_)+b]);
                                os << msg;
                            }
                            os << endl;
                        }
                        os << endl;
                    }

                    return 0;
                }
            }  // print()

            /*
             *======================================================================================================================
             *     verify()
             *
             * Purpose: 
             *     verify the integrity of a PNODE_ structure.  multiple tests are used to verify the integrity of all branch and
             *     data node structures.  the input parameter enum VERIFY_MODE may be set to either ERR_CODE, which will
             *     return an integer error code(1-25) for each of the possible detected errors, or ERR_CODE_PRINT, which will
             *     return the same numerical error code and print to ofstream& os a text error message and current data
             *     structure data node addresses/keys and branch node addresses as well.
             *
             * Usage:
             *     ofstream& os;
             *     int return_code;
             *
             *     return_code = rdx->verify(ERR_CODE, os);
             *     return_code = rdx->verify(ERR_CODE_PRINT, os);
             *
             * Returns:
             *     1. return_code = 0    - 0 if no error
             *     2. return_code = 1-25 - 1-25 for the various errors
             *
             * Parameters:
             *     VERIFY_MODE vm  - enum with possible values (ERR_CODE, ERR_CODE_PRINT)
             *     ofstream& os    - output stream
             *
             * Comments:
             *     all error messages have the verify() file name and line number included
             */

                int
            verify
                (
                    VERIFY_MODE vm,
                    ofstream& os
                )
            {
                const unsigned int FREE = 0;
                const unsigned int ALLOC = 1;

                const size_t MSG_BUF_SIZE = 256;
                char msg[MSG_BUF_SIZE];

                int tot_free_nodes;
                int tot_alloc_nodes;


                // branch node and data node free list head pointers
                BNODE *bhead;
                DNODE *dhead;

                // holds all of the allocated branch node addresses
                // unsigned long bnode_addrs[num_keys_][max_num_rdx_nodes_+1]; -> bnode_addrs[k*(max_num_rdx_nodes_+1)+n]
                static unsigned long *bnode_addrs;

                // holds all of the unallocated branch node addresses
                // unsigned long free_bnode_addrs[num_keys_][max_num_rdx_nodes_+1]; -> free_bnode_addrs[k*(max_num_rdx_nodes_+1)+n]
                static unsigned long *free_bnode_addrs;

                // holds all of the allocated data node addresses
                // unsigned long dnode_addrs[max_num_rdx_nodes_+1]; -> dnode_addrs[n]
                static unsigned long *dnode_addrs;

                // holds all of the unallocated data node addresses
                // unsigned long free_dnode_addrs[max_num_rdx_nodes_+1]; -> free_dnode_addrs[n]
                static unsigned long *free_dnode_addrs;

                // hold all of the allocated data node keys
                // unsigned char dnode_keys[num_keys_][max_num_rdx_nodes_+1][num_key_bytes_]; -> dnode_keys[(k*(max_num_rdx_nodes_+1)+n)*num_key_bytes_+b]
                static unsigned char *dnode_keys;

                // holds integer indexes(0 to max_num_rdx_nodes_) of free and allocated nodes
                // unsigned long node_index[2][max_num_rdx_nodes_+1]; -> node_index[i*(max_num_rdx_nodes_+1)+n]
                static unsigned long *node_index;

                static bool alloc = false;  // static - only first time verify() for calloc() calls


                if ( alloc == false )
                {
                    void *fptr;
                    alloc = true;


                    fptr = calloc( num_keys_ * (max_num_rdx_nodes_+1), sizeof(unsigned long));
                    bnode_addrs = (unsigned long *) fptr;
                    free_ptr_.push_back(fptr);
                    snprintf(msg, MSG_BUF_SIZE, "verify(): calloc(): unsigned long bnode_addrs[num_keys_][max_num_rdx_nodes_+1]");
                    free_msg_.push_back(msg);

                    fptr = calloc( num_keys_ * (max_num_rdx_nodes_+1), sizeof(unsigned long));
                    free_bnode_addrs = (unsigned long *) fptr;
                    free_ptr_.push_back(fptr);
                    snprintf(msg, MSG_BUF_SIZE, "verify(): calloc(): unsigned long free_bnode_addrs[num_keys_][max_num_rdx_nodes_+1];");
                    free_msg_.push_back(msg);

                    fptr = calloc( max_num_rdx_nodes_+1, sizeof(unsigned long));
                    dnode_addrs = (unsigned long *) fptr;
                    free_ptr_.push_back(fptr);
                    snprintf(msg, MSG_BUF_SIZE, "verify(): calloc(): unsigned long dnode_addrs[max_num_rdx_nodes_+1];");
                    free_msg_.push_back(msg);

                    fptr = calloc( max_num_rdx_nodes_+1, sizeof(unsigned long));
                    free_dnode_addrs = (unsigned long *) fptr;
                    free_ptr_.push_back(fptr);
                    snprintf(msg, MSG_BUF_SIZE, "verify(): calloc(): unsigned long free_dnode_addrs[max_num_rdx_nodes_+1];");
                    free_msg_.push_back(msg);

                    fptr = calloc( num_keys_ * (max_num_rdx_nodes_+1) * num_key_bytes_ , sizeof(unsigned char));
                    dnode_keys = (unsigned char *) fptr;
                    free_ptr_.push_back(fptr);
                    snprintf(msg, MSG_BUF_SIZE, "verify(): calloc(): unsigned char dnode_keys[num_keys_][max_num_rdx_nodes_+1][num_key_bytes_];");
                    free_msg_.push_back(msg);

                    fptr = calloc( 2 * (max_num_rdx_nodes_+1) , sizeof(unsigned long));
                    node_index = (unsigned long *) fptr;
                    free_ptr_.push_back(fptr);
                    snprintf(msg, MSG_BUF_SIZE, "verify(): calloc(): unsigned long node_index[2][max_num_rdx_nodes_+1];");
                    free_msg_.push_back(msg);
                }

                // accumulate all the free and allocated branch and data node addresses, the node indexes on
                // each free/alloc node and the total number of free and allocated nodes
                tot_free_nodes = 0;
                tot_alloc_nodes = 0;
                for ( int n = 0 ; n < max_num_rdx_nodes_+1 ; n++ )
                {
                    if ( rdx_.dnodes[n].alloc == 0 )
                    {
                        node_index[FREE*(max_num_rdx_nodes_+1)+tot_free_nodes] = n;
                        free_dnode_addrs[tot_free_nodes] = (unsigned long)&rdx_.dnodes[n];
                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            free_bnode_addrs[k*(max_num_rdx_nodes_+1)+tot_free_nodes] = (unsigned long)&rdx_.bnodes[n*num_keys_+k];
                        }

                        tot_free_nodes++;
                    }
                    else
                    {
                        node_index[ALLOC*(max_num_rdx_nodes_+1)+tot_alloc_nodes] = n;
                        dnode_addrs[tot_alloc_nodes] = (unsigned long)&rdx_.dnodes[n];
                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            bnode_addrs[k*(max_num_rdx_nodes_+1)+tot_alloc_nodes] = (unsigned long)&rdx_.bnodes[n*num_keys_+k];
                            memmove( &dnode_keys[(k*(max_num_rdx_nodes_+1)+tot_alloc_nodes)*num_key_bytes_+0], &rdx_.dnodes[n].key[k*(1+num_key_bytes_)+1], num_key_bytes_ );
                                   // dnode_keys[(k*(max_num_rdx_nodes_+1)+n)*num_key_bytes_+b]
                        }

                        tot_alloc_nodes++;
                    }
                }

                // print all of the free/alloc data structure addresses, node indexes and keys
                if ( vm == ERR_CODE_PRINT )
                {
                    os << "return_code = rdx->verify(ERR_CODE_PRINT, os);\n";
                    os << "Free Data Node Address(DNA)/Keys and Branch Node Addresses(BNAs)\n";
                    os << "node  DNA         BNA(s)  ";
                    for ( int k = 0 ; k < num_keys_ ; k++ )
                    {
                        snprintf(msg, MSG_BUF_SIZE, "key = %-2d  ", k);
                        os << msg;
                    }
                    os << endl;

                    for ( int n = 0 ; n < tot_free_nodes ; n++ )
                    {
                        snprintf(msg, MSG_BUF_SIZE, "%4ld  %08lx          ", node_index[FREE*(max_num_rdx_nodes_+1)+n], free_dnode_addrs[n]);
                        os << msg;
                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                           snprintf(msg, MSG_BUF_SIZE, "  %08lx", free_bnode_addrs[k*(max_num_rdx_nodes_+1)+n]);
                            os << msg;
                        }
                        os << endl;
                    }
                    os << endl;

                    os << "Allocated Data Node Address(DNA)/Keys and Branch Node Addresses(BNAs)\n";
                    os << "node  DNA         BNA(s)  ";
                    for ( int k = 0 ; k < num_keys_ ; k++ )
                    {
                        snprintf(msg, MSG_BUF_SIZE, "key = %-2d  ", k);
                        os << msg;
                    }
                    os << endl;

                    for ( int n = 0 ; n < tot_alloc_nodes ; n++ )
                    {
                        snprintf(msg, MSG_BUF_SIZE, "%4ld  %08lx          ", node_index[ALLOC*(max_num_rdx_nodes_+1)+n], dnode_addrs[n]);
                        os << msg;
                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "  %08lx", bnode_addrs[k*(max_num_rdx_nodes_+1)+n]);
                            os << msg;
                        }
                        os << endl;

                        for ( int k = 0 ; k < num_keys_ ; k++ )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "      key %-2d = ", k);
                            os << msg;
                            for ( int b = 0 ; b < num_key_bytes_ ; b++ )
                            {
                                snprintf(msg, MSG_BUF_SIZE, "%02x ", dnode_keys[(k*(max_num_rdx_nodes_+1)+n)*num_key_bytes_+b]);
                                os << msg;
                            }
                            os << endl;
                        }
                        os << endl;
                    }
                }

                // check if certain root node values are initialized to zero - there will
                // be num_keys_ branch nodes and one data node in the root node - these
                // values should remain permanently invariant at zero
                for ( int k = 0 ; k < num_keys_ ; k++ )
                {
                    if ( rdx_.bnodes[0*num_keys_+k].id != 0 )
                    {
                        if ( vm == ERR_CODE_PRINT )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 1): Root branch node with non-zero id - %d\n",
                                __FILE__, __LINE__, rdx_.bnodes[0*num_keys_+k].id);
                            os << msg;
                        }
                        return 1;
                    }

                    if ( rdx_.bnodes[0*num_keys_+k].br != 0 )
                    {
                        if ( vm == ERR_CODE_PRINT )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 2): Root branch node with non-zero br - %d\n",
                                __FILE__, __LINE__, rdx_.bnodes[0*num_keys_+k].br);
                            os << msg;
                        }
                        return 2;
                    }

                    if ( rdx_.bnodes[0*num_keys_+k].p != 0 )
                    {
                        if ( vm == ERR_CODE_PRINT )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 3): Root branch node with non-zero p - 0x%08lx\n",
                                __FILE__, __LINE__, (unsigned long)rdx_.bnodes[0*num_keys_+k].p);
                            os << msg;
                        }
                        return 3;
                    }

                    if ( rdx_.bnodes[0*num_keys_+k].b != (unsigned int)num_key_bytes_*8 )
                    {
                        if ( vm == ERR_CODE_PRINT )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 4): Root branch node with b not equal to"
                                        " num_key_bytes_*8 - %d\n",
                                __FILE__, __LINE__, rdx_.bnodes[0*num_keys_+k].b);
                            os << msg;
                        }
                        return 4;
                    }

                    if ( rdx_.bnodes[0*num_keys_+k].r != 0 )
                    {
                        if ( vm == ERR_CODE_PRINT )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 5): Root branch node with non-zero r - 0x%08lx\n",
                                __FILE__, __LINE__, (unsigned long)rdx_.bnodes[0*num_keys_+k].r);
                            os << msg;
                        }
                        return 5;
                    }

                    for ( int b = 0 ; b < num_key_bytes_+1 ; b++ )
                    {
                        if ( rdx_.dnodes[0].key[k*(1+num_key_bytes_)+b] != 0xff )
                        {
                            if ( vm == ERR_CODE_PRINT )
                            {
                                snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 6): Root data node keys not all 0xff\n",
                                    __FILE__, __LINE__);
                                os << msg;
                            }
                            return 6;
                        }
                    }
                }

                // save data node, free node, branch node and key node address queues(will be unsorted),
                // total number of free nodes and total number of allocated nodes
                for ( int n = 0 ; n < max_num_rdx_nodes_+1 ; n++ )
                {
                    if ( rdx_.dnodes[n].id != 1 )
                    {
                        if ( vm == ERR_CODE_PRINT )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 7): Data node id not 1 in node sequence number %d\n",
                                __FILE__, __LINE__, n);
                            os << msg;
                        }
                        return 7;
                    }

                    for ( int k = 0 ; k < num_keys_ ; k++ )
                    {
                        if ( rdx_.bnodes[n*num_keys_+k].nsn != (unsigned int)n )
                        {
                            if ( vm == ERR_CODE_PRINT )
                            {
                                snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 8): Branch node sequence number of key index %d"
                                            " not valid - %d\n",
                                    __FILE__, __LINE__, k, rdx_.bnodes[n*num_keys_+k].nsn);
                                os << msg;
                            }
                            return 8;
                        }
                    }

                    if ( rdx_.dnodes[n].nsn != (unsigned int)n )
                    {
                        if ( vm == ERR_CODE_PRINT )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 9): Data node sequence number not valid - %d\n",
                                __FILE__, __LINE__, rdx_.dnodes[n].nsn);
                            os << msg;
                        }
                        return 9;
                    }

                    if ( rdx_.dnodes[n].alloc != 0 && rdx_.dnodes[n].alloc != 1 )
                    {
                        if ( vm == ERR_CODE_PRINT )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 10): Data node allocated boolean not 0/1"
                                        " in node sequence number %d\n",
                                __FILE__, __LINE__, rdx_.dnodes[n].nsn);
                            os << msg;
                        }
                        return 10;
                    }
                }

                // verify that the sum of free and allocated nodes is max_num_rdx_nodes_+1 nodes
                if ( (tot_alloc_nodes + tot_free_nodes) != max_num_rdx_nodes_+1 )
                {
                    if ( vm == ERR_CODE_PRINT )
                    {
                        snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 11): Total allocated nodes(%d) + free nodes(%d)"
                                    " not equal max_num_rdx_nodes_+1(%d)\n",
                            __FILE__, __LINE__, tot_alloc_nodes, tot_free_nodes, max_num_rdx_nodes_+1);
                        os << msg;
                    }
                    return 11;
                }

                // verify that the tot_alloc_nodes equals tot_nodes
                if ( (unsigned int)tot_alloc_nodes != (rdx_.tot_nodes+1) )
                {
                    if ( vm == ERR_CODE_PRINT )
                    {
                        snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 12): Total allocated nodes(%d)"
                                    " not equal rdx_.tot_nodes(%d)\n",
                            __FILE__, __LINE__, tot_alloc_nodes, rdx_.tot_nodes);
                        os << msg;
                    }
                    return 12;
                }

                // sort data node address queue
                {
                    int n = 0;
                    vector<unsigned long> da;  // dnode_addrs[n]

                    for ( n = 0 ; n < tot_alloc_nodes ; n++ )
                    {
                        // unsigned long dnode_addrs[max_num_rdx_nodes_+1]; -> dnode_addrs[n]
                        unsigned long ul = dnode_addrs[n];
                        da.push_back(ul);
                    }

                    std::sort( da.begin(), da.end() );

                    n = 0;
                    for( vector<unsigned long>::iterator it = da.begin(); it != da.end(); ++it )
                    {
                        dnode_addrs[n] = *it;
                        n++;
                    }
                }

                // sort free node address queue
                {
                    int n = 0;
                    vector<unsigned long> fda;  // free_dnode_addrs[n]

                    for ( n = 0 ; n < tot_free_nodes ; n++ )
                    {
                        // unsigned long free_dnode_addrs[max_num_rdx_nodes_+1]; -> free_dnode_addrs[n]
                        unsigned long ul = free_dnode_addrs[n];
                        fda.push_back(ul);
                    }

                    std::sort( fda.begin(), fda.end() );

                    n = 0;
                    for( vector<unsigned long>::iterator it = fda.begin(); it != fda.end(); ++it )
                    {
                        free_dnode_addrs[n] = *it;
                        n++;
                    }
                }

                for ( int k = 0 ; k < num_keys_ ; k++ )
                {
                    // sort key branch node address queue
                    {
                        int n = 0;
                        vector<unsigned long> ba;  // bnode_addrs[]

                        for ( n = 0 ; n < tot_alloc_nodes ; n++ )
                        {
                            // unsigned long bnode_addrs[num_keys_][max_num_rdx_nodes_+1]; -> bnode_addrs[k*(max_num_rdx_nodes_+1)+n]
                            unsigned long ul = bnode_addrs[k*(max_num_rdx_nodes_+1)+n];
                            ba.push_back(ul);
                        }

                        std::sort( ba.begin(), ba.end() );

                        n = 0;
                        for( vector<unsigned long>::iterator it = ba.begin(); it != ba.end(); ++it )
                        {
                            bnode_addrs[k*(max_num_rdx_nodes_+1)+n] = *it;
                            n++;
                        }
                    }

                    // sort key free branch node queue
                    {
                        int n = 0;
                        vector<unsigned long> fba;  // free_bnode_addrs[]

                        for ( n = 0 ; n < tot_free_nodes ; n++ )
                        {
                            // unsigned long free_bnode_addrs[num_keys_][max_num_rdx_nodes_+1]; -> free_bnode_addrs[k*(max_num_rdx_nodes_+1)+n]
                            unsigned long ul = free_bnode_addrs[k*(max_num_rdx_nodes_+1)+n];
                            fba.push_back(ul);
                        }

                        std::sort( fba.begin(), fba.end() );

                        n = 0;
                        for( vector<unsigned long>::iterator it = fba.begin(); it != fba.end(); ++it )
                        {
                            free_bnode_addrs[k*(max_num_rdx_nodes_+1)+n] = *it;
                            n++;
                        }
                    }

                    // sort allocated data node keys
                    {
                        int n = 0;
                        vector<string> dk;  // dnode_keys[]

                        for ( n = 0 ; n < tot_alloc_nodes ; n++ )
                        {
                            // unsigned char dnode_keys[num_keys_][max_num_rdx_nodes_+1][num_key_bytes_]; -> dnode_keys[(k*(max_num_rdx_nodes_+1)+n)*num_key_bytes_+b]
                            string s( (char *)(&dnode_keys[(k*(max_num_rdx_nodes_+1)+0)*num_key_bytes_+(n*num_key_bytes_)]), num_key_bytes_);
                            dk.push_back(s);
                        }

                        std::sort( dk.begin(), dk.end() );

                        n = 0;
                        for( vector<string>::iterator it = dk.begin(); it != dk.end(); ++it )
                        {
                            (*it).copy( (char *)(&dnode_keys[(k*(max_num_rdx_nodes_+1)+0)*num_key_bytes_+(n*num_key_bytes_)]), num_key_bytes_ );
                            n++;
                        }
                    }
                }

                // check for multiple nodes with same key in a given key index - in sorted
                // order identical keys will show up next to each other
                for ( int k = 0 ; k < num_keys_ ; k++ )
                {
                    for ( int n = 0 ; n < tot_alloc_nodes-1 ; n++ )
                    {
                        if ( memcmp( &dnode_keys[(k*(max_num_rdx_nodes_+1)+n)*num_key_bytes_+0], &dnode_keys[(k*(max_num_rdx_nodes_+1)+n+1)*num_key_bytes_+0], num_key_bytes_ ) == 0 )
                        {
                            if ( vm == ERR_CODE_PRINT )
                            {
                                snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 13): Multiple nodes with same key of key index %d"
                                            " and key value\n   ",
                                    __FILE__, __LINE__, k);
                                os << msg;
                                for ( int b = 0 ; b < num_key_bytes_ ; b++ )
                                {
                                    snprintf(msg, MSG_BUF_SIZE, "%02x ", dnode_keys[(k*(max_num_rdx_nodes_+1)+n)*num_key_bytes_+b]);
                                    os << msg;
                                }
                                os << endl;
                            }
                            return 13;
                        }
                    }
                }

                // walk branch node free list(s) - checking for valid branch node addresses.
                // if head pointer is NULL then the free node list is empty
                for ( int k = 0 ; k < num_keys_ ; k++ )
                {
                    bhead = (BNODE *)(rdx_.bfree_head[k]);

                    // walk data node free list
                    while ( bhead != NULL )
                    {
                        void *ptr;

                        // check if current pointer is in free data node list
                        ptr = bsearch(
                                         (void *)(&bhead),
                                         (void *)(&free_bnode_addrs[k*(max_num_rdx_nodes_+1)+0]),
                                         tot_free_nodes,
                                         sizeof(unsigned long),
                                         unsigned_long_compare
                                     );
                        if ( ptr == NULL )
                        {
                            if ( vm == ERR_CODE_PRINT )
                            {
                                snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 14): Pointer in branch node free list"
                                            " with key index %d corrupted\n",
                                    __FILE__, __LINE__, k);
                                os << msg;
                            }
                            return 14;
                        }
                        bhead = (BNODE *)(bhead->p);
                    }
                }

                // walk data node free list - checking for valid data node addresses.
                // if head pointer is NULL then the free node list is empty
                dhead = (DNODE *)(rdx_.dfree_head);

                // walk data node free list
                while ( dhead != NULL )
                {
                    void *ptr;

                    // check if current pointer is in free data node list
                    ptr = bsearch(
                                     (void *)(&dhead),
                                     (void *)(&free_dnode_addrs[0]),
                                     tot_free_nodes,
                                     sizeof(unsigned long),
                                     unsigned_long_compare
                                 );
                    if ( ptr == NULL )
                    {
                        if ( vm == ERR_CODE_PRINT )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 15): Pointer in data node free list corrupted\n",
                                __FILE__, __LINE__);
                            os << msg;
                        }
                        return 15;
                    }
                    dhead = (DNODE *)(dhead->nnfp);
                }

                // for each allocated node check for valid branch and data node values
                for ( int n = 0 ; n < tot_alloc_nodes ; n++ )
                {
                    unsigned long ui;
                    void *ptr, *bptr, *dptr;


                    for ( int k = 0 ; k < num_keys_ ; k++ )
                    {
                        // check if bad branch node id
                        if ( rdx_.bnodes[n*num_keys_+k].id != 0 )
                        {
                            if ( vm == ERR_CODE_PRINT )
                            {
                                snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 16): Branch node id at key index %d and node %d not"
                                            " valid - %d\n",
                                    __FILE__, __LINE__, k, n, rdx_.bnodes[n*num_keys_+k].id);
                                os << msg;
                            }
                            return 16;
                        }

                        // check if bad parent branch indicator
                        if ( rdx_.bnodes[n*num_keys_+k].br != 0 && rdx_.bnodes[n*num_keys_+k].br != 1 )
                        {
                            if ( vm == ERR_CODE_PRINT )
                            {
                                snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 17): Branch node parent branch boolean of key index %d"
                                            " and node %d not valid - %d\n",
                                    __FILE__, __LINE__, k, n, rdx_.bnodes[n*num_keys_+k].br);
                                os << msg;
                            }
                            return 17;
                        }

                        // check if branch node parent pointers are valid - the root node(i=0) will not have a parent
                        if ( n != 0 )
                        {
                            ui = (unsigned long)rdx_.bnodes[n*num_keys_+k].p;
                            ptr = bsearch(
                                             (void *)(&ui),
                                             (void *)(&bnode_addrs[k*(max_num_rdx_nodes_+1)+0]),
                                             tot_alloc_nodes,
                                             sizeof(unsigned long),
                                             unsigned_long_compare
                                         );
                            if ( ptr == NULL )
                            {
                                if ( vm == ERR_CODE_PRINT )
                                {
                                    snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 18): Branch node parent pointer of key index %d"
                                                " and node %d corrupted\n",
                                        __FILE__, __LINE__, k, n);
                                    os << msg;
                                }
                                return 18;
                            }
                        }

                        // check if PATRICIA algorithm branch test bit number is valid
                        if ( rdx_.bnodes[n*num_keys_+k].b > (unsigned int)num_key_bytes_*8 )
                        {
                            if ( vm == ERR_CODE_PRINT )
                            {
                                snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 19): Branch node bit number branch test value of"
                                            " key index %d and node %d not valid - %d\n",
                                    __FILE__, __LINE__, k, n, rdx_.bnodes[n*num_keys_+k].b);
                                os << msg;
                            }
                            return 19;
                        }

                        // check if left child pointer is another branch or data node
                        ui = (unsigned long)rdx_.bnodes[n*num_keys_+k].l;
                        bptr = bsearch(
                                          (void *)(&ui),
                                          (void *)(&bnode_addrs[k*(max_num_rdx_nodes_+1)+0]),
                                          tot_alloc_nodes,
                                          sizeof(unsigned long),
                                          unsigned_long_compare
                                      );
                        dptr = bsearch(
                                          (void *)(&ui),
                                          (void *)(&dnode_addrs[0]),
                                          tot_alloc_nodes,
                                          sizeof(unsigned long),
                                          unsigned_long_compare
                                      );
                        if ( bptr == NULL && dptr == NULL )
                        {
                            if ( vm == ERR_CODE_PRINT )
                            {
                                snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 20): Branch node left pointer of key index %d"
                                            " and node %d corrupted\n",
                                    __FILE__, __LINE__, k, n);
                                os << msg;
                            }
                            return 20;
                        }

                        // check if right child pointer is another branch or data node
                        // - the root node(i=0) will not have a right child
                        if ( n != 0 )
                        {
                            ui = (unsigned long)rdx_.bnodes[n*num_keys_+k].r;
                            bptr = bsearch(
                                              (void *)(&ui),
                                              (void *)(&bnode_addrs[k*(max_num_rdx_nodes_+1)+0]),
                                              tot_alloc_nodes,
                                              sizeof(unsigned long),
                                              unsigned_long_compare
                                          );
                            dptr = bsearch(
                                              (void *)(&ui),
                                              (void *)(&dnode_addrs[0]),
                                              tot_alloc_nodes,
                                              sizeof(unsigned long),
                                              unsigned_long_compare
                                          );
                            if ( bptr == NULL && dptr == NULL )
                            {
                                if ( vm == ERR_CODE_PRINT )
                                {
                                    snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 21): Branch node right pointer of key index %d"
                                                " and node %d corrupted\n",
                                        __FILE__, __LINE__, k, n);
                                    os << msg;
                                }
                                return 21;
                            }
                        }

                        // check if data node parent pointers are all valid branch node addresses
                        ui = (unsigned long)rdx_.dnodes[n].p[k];
                        ptr = bsearch(
                                         (void *)(&ui),
                                         (void *)(&bnode_addrs[k*(max_num_rdx_nodes_+1)+0]),
                                         tot_alloc_nodes,
                                         sizeof(unsigned long),
                                         unsigned_long_compare
                                     );
                        if ( ptr == NULL )
                        {
                            if ( vm == ERR_CODE_PRINT )
                            {
                                snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 22): Data node parent pointer of key index %d"
                                            " and node %d corrupted\n",
                                    __FILE__, __LINE__, k, n);
                                os << msg;
                            }
                            return 22;
                        }

                        // check if all data node parent branch indicators are valid
                        if ( rdx_.dnodes[n].br[k] != 0 && rdx_.dnodes[n].br[k] != 1 )
                        {
                            if ( vm == ERR_CODE_PRINT )
                            {
                                snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 23): Data node parent branch boolean of key index %d and"
                                            " node %d not valid - %d\n",
                                    __FILE__, __LINE__, k, n, rdx_.dnodes[n].br[k]);
                                os << msg;
                            }
                            return 23;
                        }
                    }
                }

                // retrieve all keys from all data nodes and use normal search(rdx->search())
                // to find them - ignore root node keys

                // unsigned char key[num_keys_][1+num_key_bytes_]
                unsigned char *key = (unsigned char *)calloc( num_keys_ * (1+num_key_bytes_), sizeof(unsigned char) );

                for ( int n = 1 ; n < tot_alloc_nodes ; n++ )
                {
                    app_data *app_datap;


                    // get data node keys directly from allocated data node
                    for ( int k = 0 ; k < num_keys_ ; k++ )
                    {
                        key[k*(1+num_key_bytes_)+0] = 1;
                        memmove( &key[k*(1+num_key_bytes_)+1], &rdx_.dnodes[n].key[k*(1+num_key_bytes_)+1], num_key_bytes_ );
                    }

                    // search for node n keys
                    app_datap = search(key);

                    if ( app_datap == NULL )
                    {
                        if ( vm == ERR_CODE_PRINT )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 24): Data node %d searched for with it's keys not found\n",
                                __FILE__, __LINE__, n);
                            os << msg;
                        }
                        return 24;
                    }

                    // do full compare of 'app_data data;' obtained from rdx->search() and the same data obtained from rdx_.dnodes[n].data.
                    // if they do not match, error.
                    if ( memcmp( app_datap, &rdx_.dnodes[n].data, sizeof(app_data) ) != 0 )
                    {
                        if ( vm == ERR_CODE_PRINT )
                        {
                            snprintf(msg, MSG_BUF_SIZE, "rdx->verify():(file %s  line %d  ERR_CODE 25): compare of 'app_data data;' obtained from rdx->search() and"
                                        " the same data obtained from rdx_.dnodes[%d].data do not match",
                                __FILE__, __LINE__, n);
                            os << msg;
                        }
                        return 25;
                    }
                }
                free(key);

                return 0;
            }  // verify()

    };  // class MKRdxPat

}  // namespace MultiKeyRdxPat

#endif  // RDX_CPP_MKRDXPAT_H_

