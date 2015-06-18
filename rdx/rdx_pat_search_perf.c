/*
 *======================================================================================================================
 *
 * File Names:
 *     rdx_pat_search.c
 *     rdx_pat_search.h
 *     APP_DATA.h
 *
 *======================================================================================================================
 *
 * Prototypes:
 *
 *         int
 *     rdx_pat_initialize
 *         (
 *             PNODE *pnodep
 *         );
 *
 *         int
 *     rdx_pat_insert
 *         (
 *             PNODE *pnodep,
 *             unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES],
 *             DNODE **return_ptr
 *         );
 *
 *         DNODE *
 *     rdx_pat_search
 *         (
 *             PNODE *pnodep,
 *             unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
 *         );
 *
 *         DNODE *
 *     rdx_pat_remove
 *         (
 *             PNODE *pnodep,
 *             unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
 *         );
 *
 *         int
 *     rdx_pat_sort
 *         (
 *             PNODE *pnodep,
 *             DNODE ***data_nodep,
 *             unsigned int k
 *         );
 *
 *         int
 *     rdx_pat_nodes
 *         (
 *             PNODE *pnodep
 *         );
 *
 *         int
 *     rdx_pat_print
 *         (
 *             PNODE *pnodep,
 *             unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES],
 *             FILE *fp
 *         );
 *
 *         int
 *     rdx_pat_verify
 *         (
 *             PNODE *pnodep,
 *             VERIFY_MODE vm,
 *             FILE *fp
 *         );
 *
 *======================================================================================================================
 *
 * Description:
 *     Routines to support multi-key radix PATRICIA(Practical Algorithm To Retrieve Information Coded In Alphanumeric)
 *     fast search(see ref. 1).  A data structure is supported of MAX_NUM_RDX_NODES data nodes and NUM_KEYS keys per
 *     data node with keys of NUM_KEY_BYTES bytes length.  Each of these three defines may be set to any arbitrary
 *     integer value one or higher in rdx_pat_search.h(a trie of one node with one key of one byte will work, but might
 *     not be very useful).  For a trie of N keys each data node will have N branch nodes associated with it, each of
 *     the N branch nodes is associated with one of the N keys.  Again, see reference 1 on the PATRICIA algorithm for a
 *     description of what goes into the branch nodes and how traversal of a series of branch nodes leads to a unique
 *     data node.  I sometimes refer to "nodes" which are the MAX_NUM_RDX_NODES data structure nodes, each of which
 *     has one data node and N branch nodes within it.  The user would define an arbitrarily complex data structure
 *     in the APP_DATA typedef in APP_DATA.h and then specify in rdx_pat_search.h the values of the three defines
 *     noted above.  The number of actual nodes in the data structure is MAX_NUM_RDX_NODES+1.  This extra node is for
 *     the initial branch node and the initial data node with the impossible key of all 0xff's.  The number of user
 *     storable nodes is MAX_NUM_RDX_NODES.   The user declares PNODEs, each of which contains a radix PATRICIA trie of
 *     MAX_NUM_RDX_NODES nodes with NUM_KEYS keys of NUM_KEY_BYTES bytes length.
 *
 *     Each data node of N keys with key index numbers from 0 to N-1 must have keys that are unique for a given key
 *     index, e.g. for N=2 and key index numbers 0 and 1 we can insert a data node with keys (2,2)(Notation: (n,m)
 *     means key index 0 = n and key index 1 = m) successfully, however if we now try to insert a new data node with
 *     keys (2,3) this fails since the key at key index 0 - 2 - is the same as the previously inserted node key index
 *     0 key value of 2.  
 *
 *     These routines are a modification of the algorithm cited in reference 1 below.  Specifically, the upward pointers
 *     used to terminate search are not used, and are instead used to point to data nodes as trie leaves.  In multi-key
 *     search with PATRICIA the upward pointer scheme will not work since each key would have pointed to a different
 *     branch node with it's associated data structure.  In multi-key PATRICIA search all keys must lead to the same
 *     data node.  The viability of multi-key radix PATRICIA search depends on the fact that each data node insertion
 *     requires only one new branch node for each key, thus the initial allocation of N branch nodes with each data node.
 *     This implementation has several branch and data node members not used in the reference 1 PATRICIA search algorithm
 *     description.  These are for various purposes; related to changes in the original algorithm for multi-key search,
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
 *        will determine NUM_KEY_BYTES.
 *
 *     4. it is suggested that the 'unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]' array be first memset() to 0 and then
 *        the keys added with the key boolean prepended and set appropriately in one operation.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// rdx function prototypes and typedefs of PNODE, BNODE and DNODE
#include "rdx_pat_search_perf.h"

// max number of key bits
#define MAX_KEY_BITS          NUM_KEY_BYTES*8

// given a key and bit number (bits numbered 0 from right) will return that bit
    static unsigned int
gbit
    (
        unsigned char *key,
        unsigned int bit_num
    )
{
    static unsigned int byte;
    static unsigned int bit;
    static unsigned char mask;


    /*
     * the byte index set here assumes one extra prefix byte in the input parameter key(for the 0xff of the root
     * node impossible key). thus, data nodes with keys of NUM_KEY_BYTES will have a 0 byte prefix added, and the
     * byte index set here is not 0-NUM_KEY_BYTES but 1-NUM_KEY_BYTES+1. e.g. if NUM_KEY_BYTES=1 and bit_num=0
     * then byte is set to 1 not 0.  if NUM_KEY_BYTES=16 and bit_num=0 then byte is set to 16 not 15.
     */
    mask = 1;
    byte = NUM_KEY_BYTES - bit_num/8;
    mask <<= bit_num%8;
    bit = key[byte] & mask;
    bit >>= bit_num%8;

    return bit;
}


/*
 *======================================================================================================================
 *     rdx_pat_initialize()
 *
 * Purpose: 
 *     Initialize the defined PNODE data structure.  This includes setting the data and branch node sequence numbers and
 *     data node allocated status(to 0).  Set total allocated nodes to 0, and allocated status of root node to 1.  Set
 *     the branch node free list up for each key and the data node free list.
 *
 * Usage:
 *     PNODE data;
 *     int rdx_size;
 *
 *     rdx_size = rdx_pat_initialize(&data);
 *
 * Returns:
 *     sizeof PNODE
 *     0  -  if MAX_NUM_RDX_NODES, NUM_KEYS or NUM_KEY_BYTES is less than 1
 *
 * Parameters:
 *     PNODE *pnodep - pointer to the PNODE structure
 *
 * Comments:
 *     1. the entire data structure is initialized to 0xf0 in order to help expose any pointer errors.  the entire data
 *        node space is likewise initialized to 0xf0 which might expose misuse of the data.  this should not affect
 *        anything.
 *
 *     2. remember that the data structure has MAX_NUM_RDX_NODES+1 nodes(each of which has one data and NUM_KEYS branch
 *        nodes).  the first is the always allocated impossible key root node.  the rest are the MAX_NUM_RDX_NODES user
 *        usable nodes.
 *
 *     3. certain pointers in the root node, such as parent pointers, are set to zero and permanently remain so since
 *        they have nothing to point to.  in root node printouts they should be zero.
 */

    int
rdx_pat_initialize
    (
        PNODE *pnodep
    )
{
    if ( MAX_NUM_RDX_NODES < 1 )
    {
        return 0;
    }

    if ( NUM_KEYS < 1 )
    {
        return 0;
    }

    if ( NUM_KEY_BYTES < 1 )
    {
        return 0;
    }

    /*
     * set full data struct to 0xf0 - shouldn't affect anything, but if bug
     * then 0xf0 pattern might start showing up in various output.  this will
     * include the entire data struct APP_DATA
     */
    memset(pnodep, 0xf0, sizeof(PNODE));

    // set data and branch node sequence numbers and node allocated status
    for ( int n = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
    {
        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            pnodep->bnodes[n][k].nsn = n;
        }

        pnodep->dnodes[n].nsn = n;
        pnodep->dnodes[n].alloc = 0;
    }
    pnodep->dnodes[0].alloc = 1; // root impossible key(0xff) node

    // reset total allocated nodes
    pnodep->tot_nodes = 0;

    // for each key set various branch/data/head fields and pointers
    for ( int k = 0 ; k < NUM_KEYS ; k++ )
    {
        // set rdx trie head pointer(s)
        pnodep->head[k] = &pnodep->bnodes[0][k];

        // init head branch node
        pnodep->bnodes[0][k].id = 0;
        pnodep->bnodes[0][k].br = 0;
        pnodep->bnodes[0][k].p = NULL;
        pnodep->bnodes[0][k].b = NUM_KEY_BYTES*8;
        pnodep->bnodes[0][k].l = &pnodep->dnodes[0];
        pnodep->bnodes[0][k].r = NULL;

        // build the branch node free list
        pnodep->bfree_head[k] = &pnodep->bnodes[1][k];
        for ( int n = 1 ; n < MAX_NUM_RDX_NODES ; n++ )
        {
            pnodep->bnodes[n][k].id = 0;
            pnodep->bnodes[n][k].br = 0;
            pnodep->bnodes[n][k].p = &pnodep->bnodes[n+1][k];
            pnodep->bnodes[n][k].b = 0;
            pnodep->bnodes[n][k].l = NULL;
            pnodep->bnodes[n][k].r = NULL;
        }
        pnodep->bnodes[MAX_NUM_RDX_NODES][k].id = 0;
        pnodep->bnodes[MAX_NUM_RDX_NODES][k].br = 0;
        pnodep->bnodes[MAX_NUM_RDX_NODES][k].p = NULL;
        pnodep->bnodes[MAX_NUM_RDX_NODES][k].b = 0;
        pnodep->bnodes[MAX_NUM_RDX_NODES][k].l = NULL;
        pnodep->bnodes[MAX_NUM_RDX_NODES][k].r = NULL;

        // init data node fields
        for ( int n = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
        {
            pnodep->dnodes[n].br[k] = 0;
            pnodep->dnodes[n].p[k] = 0;
            memset(&pnodep->dnodes[n].key[k][0], 0, NUM_KEY_BYTES+1);
        }

        // set data node parent pointers of root node to branch nodes for each key
        pnodep->dnodes[0].p[k] = &pnodep->bnodes[0][k];

        // init head data node key to impossible value, (the extra high order 0xff byte).
        memset(&pnodep->dnodes[0].key[k][0], 0xff, NUM_KEY_BYTES+1);
    }

    // build the data node free list - use DNODE nnfp as pointer to next node
    pnodep->dnodes[0].id = 1;

    pnodep->dfree_head = &pnodep->dnodes[1];

    for ( int n = 1 ; n < MAX_NUM_RDX_NODES ; n++ )
    {
        pnodep->dnodes[n].nnfp = (BNODE *)&pnodep->dnodes[n+1];
        pnodep->dnodes[n].id = 1;
    }
    pnodep->dnodes[MAX_NUM_RDX_NODES].nnfp = NULL;
    pnodep->dnodes[MAX_NUM_RDX_NODES].id = 1;

    return sizeof(PNODE);
}


/*
 *======================================================================================================================
 *     rdx_pat_insert()
 *
 * Purpose: 
 *     remove a data node from the free list in the PNODE data structure and insert this node into the trie with keys
 *     key[NUM_KEYS][1+NUM_KEY_BYTES].  the second subscript first byte for all keys(key[k][0]) is the key boolean.
 *     the key boolean should always be set to 1 since all keys are needed.  the actual key bytes (key[k][1 to
 *     NUM_KEY_BYTES]) follow the key boolean.  each key must be unique within it's key index(0 to NUM_KEYS-1).
 *
 * Usage:
 *     PNODE data;
 *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES];
 *     DNODE *dnp;
 *     int return_code;
 *
 *     return_code = rdx_pat_insert(&data, key, &dnp);
 *
 * Returns:
 *     1. if insertion is successful then set return_code to 0 and set dnp to point to the data node
 *     2. if any key is found to already exist then set return_code to 1 and set dnp to point to the data node
 *     3. if no data nodes are on the free list then set return_code to 2 and set dnp to NULL
 *     4. if any key boolean is not 1 set return_code to 3 and set dnp to NULL
 *
 * Parameters:
 *     PNODE *pnodep                                - pointer to the PNODE structure
 *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES] - NUM_KEYS keys of a one byte key boolean and NUM_KEY_BYTES key bytes
 *     DNODE **return_ptr                           - pointer to pointer to the inserted data node
 *                                                    or NULL if insertion fails
 *
 * Comments:
 *
 */

    int
rdx_pat_insert
    (
        PNODE *pnodep,
        unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES],
        DNODE **return_ptr
    )
{
    /*
     * during search p[] and c[] are successively set to parent and child nodes down the trie.
     * if a new key is to be inserted bna will be the branch node address linked between p[] and c[]
     * and dna will be the data node address linked as the other child of bna.  NUM_KEYS branch nodes
     * are always added with one data node.  lr[k] is used to set the br indicator in all nodes - lr[k]=1:
     * node is a right link of its parent, lr[k]=0: node is a left link of its parent.  key_bit is the
     * leftmost bit that key is different from terminating key actually found in trie.
     */
    BNODE *p[NUM_KEYS], *bna[NUM_KEYS];
    DNODE *dna;
    unsigned int lr[NUM_KEYS];
    unsigned int key_bit[NUM_KEYS];

    /*
     * c[] is declared as BNODE * but is cast to DNODE * at the end of a search.
     * this is because all searches go through a series of BNODES and end at a DNODE.
     * the determination of search termination is made by examining c[k]->id == 1,
     * which indicates a data node.  since c[k] is declared a BNODE * and the id
     * field is in both node types, then the id field must be at the start of the
     * typedef and in the same relative position in order to get the correct value.
     */
    BNODE *c[NUM_KEYS];

    unsigned char ky[NUM_KEYS][NUM_KEY_BYTES+1];


    /*
     * if any key is found in an already existind data node then return
     * node in return_ptr and set return code to 1.  if any key boolean is not
     * 1 return NULL in return_ptr and set return code to 3.
     */
    for ( int k = 0 ; k < NUM_KEYS ; k++ )
    {
        if ( key[k][0] != 1 )
        {
            *return_ptr = NULL;
            return 3;
        }

        // copy key to storage with extra byte for comparison
        ky[k][0] = 0;
        memmove( &ky[k][1], &key[k][1], NUM_KEY_BYTES );

        // search for key in rdx trie
        c[k] = (BNODE *)pnodep->head[k]->l;
        while ( c[k]->id == 0 )
        {
            c[k] = ( gbit( ky[k], c[k]->b ) ) ? (BNODE *)c[k]->r : (BNODE *)c[k]->l ;
        }

        if ( memcmp( ky[k], ((DNODE *)c[k])->key[k], NUM_KEY_BYTES+1 ) == 0 )
        {
            *return_ptr = (DNODE *)c[k];
            return 1;
        }
    }

    // if no data nodes free set return_ptr to NULL and return 2
    for ( int k = 0 ; k < NUM_KEYS ; k++ )
    {
        if ( pnodep->bfree_head[k] == NULL || pnodep->dfree_head == NULL )
        {
        *return_ptr = NULL;
        return 2;
        }
    }

    // get free node from free list
    dna = pnodep->dfree_head;
    pnodep->dfree_head = (DNODE *)pnodep->dfree_head->nnfp;
    pnodep->tot_nodes++;

    // insert new data node with NUM_KEYS keys
    for ( int k = 0 ; k < NUM_KEYS ; k++ )
    {
        /*
         * key not found in rdx trie: find leftmost bit that key is different from terminating
         * key actually found in trie.
         *
         * NOTE: the extra high order byte should insure that any time we arrive at this code
         *       ((DNODE *)c[k])->key[k] and key are different.  the MAX_KEY_BITS bit will be the
         *       first bit of the extra byte.  if the keys are not different before or at bit zero
         *       (the furthest right most bit) then this statement will call gbit() with a negative
         *       value of key_bit and a memory violation will probably result or at the very least
         *       wholly incorrect insertion.
         */
        key_bit[k] = MAX_KEY_BITS;
        while ( gbit( ((DNODE *)c[k])->key[k], key_bit[k] ) == gbit( ky[k], key_bit[k] ) )
        {
            key_bit[k]--;
        }

        // allocate new nodes from free list
        bna[k] = pnodep->bfree_head[k];
        pnodep->bfree_head[k] = ((BNODE *)pnodep->bfree_head[k]->p);

        /*
         * search for location to insert new key - c[k]->id != 0 new node will be at external trie
         * node insertion, c[k]->b <= key_bit new node will be internal trie node insertion
         */
        lr[k] = 0;
        p[k] = pnodep->head[k];
        c[k] = ((BNODE *)pnodep->head[k]->l);
        while ( c[k]->id == 0 && c[k]->b > key_bit[k] )
        {
            p[k] = c[k];
            if ( gbit( ky[k], c[k]->b ) )
            {
                c[k] = ((BNODE *)c[k]->r);
                lr[k] = 1;
            }
            else
            {
                c[k] = ((BNODE *)c[k]->l);
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
        if ( gbit( ky[k], bna[k]->b ) )
        {
            bna[k]->l = (void *)c[k];
            bna[k]->r = (void *)dna;
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
            bna[k]->l = (void *)dna;
            bna[k]->r = (void *)c[k];
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

        // set child key
        memmove( dna->key[k], ky[k], NUM_KEY_BYTES+1 );

        // set back pointer
        dna->p[k] = bna[k];
    }

    // set node to allocated status
    dna->alloc = 1;

    // new data node insertion successful, set return values
    *return_ptr = dna;
    return 0;
}


/*
 *======================================================================================================================
 *     rdx_pat_search()
 *
 * Purpose: 
 *     search trie for the data node with the keys key[NUM_KEYS][1+NUM_KEY_BYTES].  the second subscript first byte for
 *     all keys(key[k][0]) is the key boolean.  the key boolean should be set to 1(use the key) or 0(do not use key).
 *     the actual key bytes(key[k][1 to NUM_KEY_BYTES]) follow the key boolean.  thus, 1 to NUM_KEYS keys may be used.
 *     only one key is necessary to find the data node.  each key must be unique within it's key
 *     index(0 to NUM_KEYS-1).
 *
 * Usage:
 *     PNODE data;
 *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES];
 *     DNODE *dnp;
 *
 *     dnp = rdx_pat_search(&data, key);
 *
 * Returns:
 *     data node pointer if search is successful or NULL if not
 *    
 * Parameters:
 *     PNODE *pnodep                                - pointer to the PNODE structure
 *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES] - NUM_KEYS keys of a one byte key boolean and NUM_KEY_BYTES key bytes
 *
 * Comments:
 *     e.g. set NUM_KEYS=3 and NUM_KEY_BYTES=4
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

    DNODE *
rdx_pat_search
    (
        PNODE *pnodep,
        unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
    )
{
    int n;

    BNODE *c;
    BNODE *csav;

    unsigned char ky[NUM_KEYS][NUM_KEY_BYTES+1];

    bool firsttime = true;


    n = 0;
    for ( int k = 0 ; k < NUM_KEYS ; k++ )
    {
        if ( key[k][0] == 0 )
        {
            continue;
        }
        else if ( key[k][0] == 1 )
        {
            n++;
        }
        else
        {
            return NULL;
        }

        // copy key to storage with extra byte for comparison
        ky[k][0] = 0;
        memmove( &ky[k][1], &key[k][1], NUM_KEY_BYTES );

        // search for key in rdx trie
        c = (BNODE *)pnodep->head[k]->l;
        while ( c->id == 0 )
        {
            c = ( gbit( ky[k], c->b ) ) ? (BNODE *)c->r : (BNODE *)c->l ;
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
                return NULL;
            }
        }

        // if key not found return NULL
        if ( memcmp( ky[k], ((DNODE *)c)->key[k], NUM_KEY_BYTES+1 ) != 0 )
        {
            return NULL;
        }
    }

    if ( n == 0 )
    {
        return NULL;
    }

    // success - all keys found in the same data node - return data node pointer
    return (DNODE *)c;
}


/*
 *======================================================================================================================
 *     rdx_pat_remove()
 *
 * Purpose: 
 *     remove trie data node with the keys key[NUM_KEYS][1+NUM_KEY_BYTES].  the second subscript first byte for all
 *     keys(key[k][0]) is the key boolean.  the key boolean should be set to 1(use the key) or 0(do not use key).
 *     the actual key bytes(key[k][1 to NUM_KEY_BYTES]) follow the key boolean.  thus, 1 to NUM_KEYS keys may be used.
 *     only one key is necessary to remove the data node.  each key must be unique within it's key
 *     index(0 to NUM_KEYS-1).
 *
 * Usage:
 *     PNODE data;
 *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES];
 *     DNODE *dnp;
 *
 *     dnp = rdx_pat_remove(&data, key);
 *
 * Returns:
 *     DNODE *dnp - pointer to the data node removed or NULL if no data node found
 *
 * Parameters:
 *     PNODE *pnodep                                - pointer to the PNODE structure
 *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES] - NUM_KEYS keys of a one byte key boolean and NUM_KEY_BYTES key bytes
 *
 * Comments:
 *     e.g. set NUM_KEYS=3 and NUM_KEY_BYTES=4
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
 */

    DNODE *
rdx_pat_remove
    (
        PNODE *pnodep,
        unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
    )
{
    int n;

    // other child pointer
    BNODE *oc;

    BNODE *c;
    BNODE *csav;

    unsigned char ky[NUM_KEYS][NUM_KEY_BYTES+1];

    bool firsttime = true;


    n = 0;
    for ( int k = 0 ; k < NUM_KEYS ; k++ )
    {
        if ( key[k][0] == 0 )
        {
            continue;
        }
        else if ( key[k][0] == 1 )
        {
            n++;
        }
        else
        {
            return NULL;
        }

        // copy key to storage with extra byte for comparison
        ky[k][0] = 0;
        memmove( &ky[k][1], &key[k][1], NUM_KEY_BYTES );

        // search for key in rdx trie
        c = ((BNODE *)pnodep->head[k]->l);
        while ( c->id == 0 )
        {
            c = ( gbit( ky[k], c->b ) ) ? ((BNODE *)c->r) : ((BNODE *)c->l) ;
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
                return NULL;
            }
        }

        // if key not found return NULL
        if ( memcmp( ky[k], ((DNODE *)c)->key[k], NUM_KEY_BYTES+1 ) != 0 )
        {
            return NULL;
        }
    }

    if ( n == 0 )
    {
        return NULL;
    }

    // for each key reset pointers for NUM_KEYS branch nodes and the data node
    for ( int k = 0 ; k < NUM_KEYS ; k++ )
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
        ((BNODE *)((DNODE *)c)->p[k])->p = pnodep->bfree_head[k];
        pnodep->bfree_head[k] = ((DNODE *)c)->p[k];
    }

    // set node to free status
    ((DNODE *)c)->alloc = 0;

    // return data node to free list
    ((DNODE *)c)->nnfp = pnodep->dfree_head;
    pnodep->dfree_head = (DNODE *)c;

    // decrement total allocated nodes
    pnodep->tot_nodes--;

    // set return pointer to removed node APP_DATA
    return (DNODE *)c;
}


/*
 *======================================================================================================================
 *     rdx_pat_sort()
 *
 * Purpose: 
 *     sort data nodes in PNODE data structure by the key index k in key ascending order
 *
 * Usage:
 *     PNODE data;
 *     DNODE **data_nodep;
 *     unsigned int k;
 *     int return_code;
 *
 *     return_code = rdx_pat_sort(&data, &data_nodep, k);
 *
 * Returns:
 *     int return_code     - the number of sorted entries in nodes.  if no keys return 0.  if k outside 0 to NUM_KEYS-1
 *                           return -1.
 *     DNODE **data_nodep  - an array of pointer to pointer to DNODEs.
 *
 * Parameters:
 *     PNODE *pnodep       - pointer to the PNODE structure
 *     DNODE ***data_nodep - pointer to pointer to pointer to DNODEs
 *     unsigned int k      - key index(0 to NUM_KEYS-1)
 *
 * Comments:
 *     (DNODE *)data_nodep[0 to return_code-1]          - pointers to DNODEs
 *     (DNODE *)data_nodep[0 to return_code-1]->data.id - to access node data in APP_DATA
 *     (DNODE *)data_nodep[0 to return_code-1]->key[k]  - to access key values in PNODE
 *
 */

// recursive routine to descend rdx trie
    static void
rdx_pat_rec
    (
        PNODE *pnodep,
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
        rdx_pat_rec(pnodep, (BNODE *)bnode_ptr->l);
        rdx_pat_rec(pnodep, (BNODE *)bnode_ptr->r);
    }
    else
    {
        pnodep->node_ptrs[pnodep->node_ptrs_cnt++] = (DNODE *)bnode_ptr;
    }
}

    int
rdx_pat_sort
    (
        PNODE *pnodep,
        DNODE ***data_nodep,
        unsigned int k
    )
{
    if ( k > NUM_KEYS-1 )
    {
        return -1;
    }

    /*
     * set full node_ptrs array to 0xf0 - shouldn't affect anything - then 0xf0 pattern
     * will be in array elements not allocated - makes debugging easier
     */
    memset( pnodep->node_ptrs, 0xf0, sizeof(pnodep->node_ptrs) );

    pnodep->node_ptrs_cnt = 0;

    rdx_pat_rec(pnodep, pnodep->head[k]);

    *data_nodep = pnodep->node_ptrs;

    return pnodep->node_ptrs_cnt-1;
}


/*
 *======================================================================================================================
 *     rdx_pat_nodes()
 *
 * Purpose: 
 *     return the number of allocated data nodes in the trie
 *
 * Usage:
 *     PNODE data;
 *     int n;
 *
 *     n = rdx_pat_nodes(&data);
 *
 * Returns:
 *     int n - number of allocated nodes in the trie(0-MAX_NUM_RDX_NODES)
 *
 * Parameters:
 *     PNODE *pnodep - pointer to the PNODE structure
 *
 * Comments:
 *     the number of actual nodes in the trie is MAX_NUM_RDX_NODES+1.  the extra node is the always allocated
 *     impossible key node with key(s) 0xff.  the number of user available nodes is MAX_NUM_RDX_NODES.  this
 *     function returns only the number of user allocated nodes in the trie.
 *
 */

    int
rdx_pat_nodes
    (
        PNODE *pnodep
    )
{
    return pnodep->tot_nodes;
}


/*
 *======================================================================================================================
 *     rdx_pat_print()
 *
 * Purpose: 
 *     Used in two modes:
 *         a. the second arg is NULL
 *
 *            print all the data in branch and data nodes for the entire trie.  for each data node allocated there
 *            will be NUM_KEYS new branch nodes allocated.  thus, for n data nodes allocated, there will be n*NUM_KEYS
 *            branch nodes printed and n data nodes printed.  the path along branch nodes to data nodes is not
 *            demonstrated.  useful only for small tries for debugging purposes.
 *
 *         b. the second arg is an array of 'unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]' of an existing allocated
 *            data node
 *
 *            print the data node and each branch node for each key(1-NUM_KEYS) that leads to the found data node.
 *            the application data is not printed, only the trie structure data.  the number of branch nodes printed
 *            is indeterminate; it depends on the order of the data node insertion.  as few as one branch node and
 *            as many as n branch nodes(n being the total number of data nodes already inserted) could be printed.
 *
 *     Setting 'unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]':
 *
 *     the second subscript first byte for all keys(key[k][0]) is the key boolean.  the key boolean should be set to
 *     1(use the key) or 0(do not use key).  the actual key bytes(key[k][1 to NUM_KEY_BYTES]) follow the key boolean.
 *     thus, 1 to NUM_KEYS keys may be used.  only one key is necessary to find the data node.  each key must be unique
 *     within it's key index(0-(NUM_KEYS-1)).
 *
 * Usage:
 *     PNODE data;
 *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES];
 *     FILE *fp;
 *     int return_code;
 *
 *     return_code = rdx_pat_print(&data, NULL, fp);
 *     return_code = rdx_pat_print(&data, key, fp);
 *
 * Returns:
 *     int return_code - 0 on success and 1 if data node for key[][] not found
 *
 * Parameters:
 *     PNODE *pnodep                                - pointer to the PNODE structure
 *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES] - NUM_KEYS keys of a one byte key boolean and NUM_KEY_BYTES key bytes
 *     FILE *fp                                     - file pointer for output
 *
 * Comments:
 *     e.g. set NUM_KEYS=3 and NUM_KEY_BYTES=4
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
 */

    int
rdx_pat_print
    (
        PNODE *pnodep,
        unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES],
        FILE *fp
    )
{
    unsigned char ky[NUM_KEY_BYTES+1];

    BNODE *c;
    DNODE *dnodep;


    if ( key == NULL )
    {
        int tot_nodes;

        fprintf(fp, "==========\n\n");
        fprintf(fp, "rdx_pat_print(, NULL,) called with NULL second arg.\n\n");
        fprintf(fp, "print all the data in branch and data nodes for the entire trie.  for each data node allocated there\n"
                    "will be NUM_KEYS new branch nodes allocated.  thus, for n data nodes allocated, there will be n*NUM_KEYS\n"
                    "branch nodes printed and n data nodes printed.  the path along branch nodes to data nodes is not\n"
                    "demonstrated.  useful only for small tries for debugging purposes.\n\n");

        tot_nodes = rdx_pat_nodes(pnodep);

        fprintf(fp, "Total nodes allocated: %d  Number of keys: %d  Key length: %d\n\n\n",
            tot_nodes, NUM_KEYS, NUM_KEY_BYTES);
        fprintf(fp, "NODE n\n\n");
        fprintf(fp, "Branch Node Format:\n");
        fprintf(fp, "  key = (key index into key array in data node)\n");
        fprintf(fp, "  nsn = (node sequence number, 0-MAX_NUM_RDX_NODES+1)\n");
        fprintf(fp, "   id = (node id - 0 = branch node, 1 - data node)\n");
        fprintf(fp, "  bna = (branch node address)\n");
        fprintf(fp, "   br = (left/right branch of parent - 0 = left, 1 = right)\n");
        fprintf(fp, "    p = (parent address)\n");
        fprintf(fp, "    b = (PATRICIA key bit(0 on right) at which this branch node distinguishes keys)\n");
        fprintf(fp, "    l = (address of left node child)\n");
        fprintf(fp, "    r = (address of right node child)\n");
        fprintf(fp, "\n");
        fprintf(fp, "Data Node Format:\n");
        fprintf(fp, "   id = (node id - 0 = branch node, 1 - data node)\n");
        fprintf(fp, "  dna = (data node address)\n");
        fprintf(fp, "   br = (left/right branch of parent - 0 = left, 1 = right)\n");
        fprintf(fp, "    p = (parent address)\n");
        fprintf(fp, "  nsn = (node sequence number, 0-MAX_NUM_RDX_NODES+1)\n");
        fprintf(fp, "alloc = (0-node not allocated, 1-node allocated)\n");
        fprintf(fp, "key n = (hex key value)\n");
        fprintf(fp, "      .\n");
        fprintf(fp, "      .\n");
        fprintf(fp, "      .\n\n\n");

        for ( int n = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
        {
            if ( pnodep->dnodes[n].alloc != 1 )
            {
                continue;
            }

            fprintf(fp, "NODE %d\n\n", n);

            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%7s %18d  ", "  key =", k);
            }
            fprintf(fp, "\n");

            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%7s %18d  ", "  nsn =", pnodep->bnodes[n][k].nsn);
            }
            fprintf(fp, "\n");

            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%7s %18d  ", "   id =", pnodep->bnodes[n][k].id);
            }
            fprintf(fp, "\n");

            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%7s %18p  ", "  bna =", (void *)&pnodep->bnodes[n][k]);
            }
            fprintf(fp, "\n");

            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%7s %18d  ", "   br =", pnodep->bnodes[n][k].br);
            }
            fprintf(fp, "\n");

            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%7s %18p  ", "    p =", (void *)pnodep->bnodes[n][k].p);
            }
            fprintf(fp, "\n");

            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%7s %18d  ", "    b =", pnodep->bnodes[n][k].b);
            }
            fprintf(fp, "\n");

            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%7s %18p  ", "    l =", (void *)pnodep->bnodes[n][k].l);
            }
            fprintf(fp, "\n");

            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%7s %18p  ", "    r =", (void *)pnodep->bnodes[n][k].r);
            }
            fprintf(fp, "\n\n");

            fprintf(fp, "%7s %18d  ", "   id =", pnodep->dnodes[n].id);
            fprintf(fp, "\n");
            fprintf(fp, "%7s %18p  ", "  dna =", (void *)&pnodep->dnodes[n]);
            fprintf(fp, "\n");
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%7s %18d  ", "   br =", pnodep->dnodes[n].br[k]);
            }
            fprintf(fp, "\n");

            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%7s %18p  ", "    p =", (void *)pnodep->dnodes[n].p[k]);
            }
            fprintf(fp, "\n");

            fprintf(fp, "%7s %18d  ", "  nsn =", pnodep->dnodes[n].nsn);
            fprintf(fp, "\n");
            fprintf(fp, "%7s %18d  ", "alloc =", pnodep->dnodes[n].alloc);
            fprintf(fp, "\n");
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "  key %2d = ", k);
                for ( int b = 0 ; b < NUM_KEY_BYTES ; b++ )
                {
                    fprintf(fp, "%02x ", pnodep->dnodes[n].key[k][b+1]);
                }
                fprintf(fp, "\n");
            }
            fprintf(fp, "\n");
        }

        return 0;
    }
    else
    {
        fprintf(fp, "==========\n\n");
        fprintf(fp, "rdx_pat_print(, key,) called with key[][] second arg.\n\n");
        fprintf(fp, "print the data node and each branch node for each key(1-NUM_KEYS) that leads to the found data node.\n"
                    "the application data is not printed, only the trie structure data.  the number of branch nodes printed\n"
                    "is indeterminate; it depends on the order of the data node insertion.  as few as one branch node and\n"
                    "as many as n branch nodes(n being the total number of data nodes already inserted) could be printed.\n\n");

        fprintf(fp, "Search for data node with key(s)(includes key boolean):\n");
        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            fprintf(fp, "  key %2d = ", k);
            for ( int b = 0 ; b < 1+NUM_KEY_BYTES ; b++ )
            {
                fprintf(fp, "%02x ", key[k][b]);
            }
            fprintf(fp, "\n");
        }
        fprintf(fp, "\n");

        fprintf(fp, "Branch Node Format:\n");
        fprintf(fp, "  nsn = (node sequence number, 0-MAX_NUM_RDX_NODES+1)\n");
        fprintf(fp, "   id = (node id - 0 = branch node, 1 - data node)\n");
        fprintf(fp, "  bna = (branch node address)\n");
        fprintf(fp, "   br = (left/right branch of parent - 0 = left, 1 = right)\n");
        fprintf(fp, "    p = (parent address)\n");
        fprintf(fp, "    b = (PATRICIA key bit(0 on right) at which this branch node distinguishes keys)\n");
        fprintf(fp, "    l = (address of left node child)\n");
        fprintf(fp, "    r = (address of right node child)\n");
        fprintf(fp, "\n");
        fprintf(fp, "Data Node Format:\n");
        fprintf(fp, "   id = (node id - 0 = branch node, 1 - data node)\n");
        fprintf(fp, "  dna = (data node address)\n");
        fprintf(fp, "   br = (left/right branch of parent - 0 = left, 1 = right)\n");
        fprintf(fp, "    p = (parent address)\n");
        fprintf(fp, "  nsn = (node sequence number, 0-MAX_NUM_RDX_NODES+1)\n");
        fprintf(fp, "alloc = (0-node not allocated, 1-node allocated)\n");
        fprintf(fp, "key n = (hex key value)\n");
        fprintf(fp, "      .\n");
        fprintf(fp, "      .\n");
        fprintf(fp, "      .\n\n\n");

        if ( (dnodep = rdx_pat_search(pnodep, key)) == NULL )
        {
            fprintf(fp, "rdx_pat_print():(file %s  line %d): Key not found.\n", __FILE__, __LINE__);
            return 1;
        }

        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            // copy key to storage with extra byte for comparison
            ky[0] = 0;
            memmove( &ky[1], &key[k][1], NUM_KEY_BYTES );

            fprintf(fp, "==========\n");
            fprintf(fp, "  key %2d = ", k);
            for ( int b = 0 ; b < 1+NUM_KEY_BYTES ; b++ )
            {
                fprintf(fp, "%02x ", key[k][b]);
            }
            fprintf(fp, "\n\n");

            c = (BNODE *)pnodep->head[k];
            while ( c->id == 0 )
            {
                fprintf(fp, "  nsn = %d\n", c->nsn);
                fprintf(fp, "   id = %d\n", c->id);
                fprintf(fp, "  bna = %p\n", (void *)c);
                fprintf(fp, "   br = %d\n", c->br);
                fprintf(fp, "    p = %p\n", (void *)c->p);
                fprintf(fp, "    b = %-3d\n", c->b);
                fprintf(fp, "    l = %p\n", (void *)c->l);
                fprintf(fp, "    r = %p\n", (void *)c->r);
                fprintf(fp, "\n");

                c = ( gbit( ky, c->b ) ) ? (BNODE *)c->r : (BNODE *)c->l ;
            }

            fprintf(fp, "   id = %d\n", ((DNODE *)c)->id);
            fprintf(fp, "%7s %18p\n", "  dna =", (void *)c);
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%7s %18d  ", "   br =", ((DNODE *)c)->br[k]);
            }
            fprintf(fp, "\n");

            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%7s %18p  ", "    p =", (void *)((DNODE *)c)->p[k]);
            }
            fprintf(fp, "\n");

            fprintf(fp, "  nsn = %d\n", ((DNODE *)c)->nsn);
            fprintf(fp, "alloc = %d\n", ((DNODE *)c)->alloc);
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "  key %2d = ", k);
                for ( int b = 0 ; b < 1+NUM_KEY_BYTES ; b++ )
                {
                    fprintf(fp, "%02x ", key[k][b]);
                }
                fprintf(fp, "\n");
            }
            fprintf(fp, "\n");
        }

        return 0;
    }
}


/*
 *======================================================================================================================
 *     rdx_pat_verify()
 *
 * Purpose: 
 *     verify the integrity of a PNODE structure.  many tests are used to verify the integrity of all branch and
 *     data node structures.  the input parameter enum VERIFY_MODE may be set to either ERR_CODE, which will have
 *     rdx_pat_verify() return an integer error code(1-25) for each of the possible detected errors, or ERR_CODE_PRINT,
 *     which will return the same numerical error code and print to FILE *fp a text error message and current data
 *     structure data node addresses/keys and branch node addresses as well.
 *
 * Usage:
 *     PNODE data;
 *     FILE *fp;
 *     int return_code;
 *
 *     return_code = rdx_pat_verify(&data, ERR_CODE, fp);
 *     return_code = rdx_pat_verify(&data, ERR_CODE_PRINT, fp);
 *
 * Returns:
 *     int return_code - integer numerical error code, 0 if no error or 1-25 for the many possible detected errors
 *
 * Parameters:
 *     PNODE *pnodep  - pointer to the PNODE structure
 *     VERIFY_MODE vm - enum with possible values (ERR_CODE, ERR_CODE_PRINT)
 *     FILE *fp       - file pointer for output
 *
 * Comments:
 *     1. all error messages have the file name and line number included so as to make finding the exact source
 *        location in rdx_pat_verify() easy
 *     2. scan the rdx_pat_verify() routine for "return " to find all the error numbers and the msgs associated
 *        with them
 *
 */

// used to compare node addresses stored in unsigned longs
    static int
unsigned_int_compare
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
}

// used to compare keys
    static int
string_compare
    (
        const void *a,
        const void *b
    )
{
    if ( memcmp( (unsigned char *)a,  (unsigned char *)b, NUM_KEY_BYTES ) == -1 )
    {
        return -1;
    }
    else if ( memcmp( (unsigned char *)a,  (unsigned char *)b, NUM_KEY_BYTES ) == 1 )
    {
        return 1;
    }

    return 0;
}

const unsigned int FREE = 0;
const unsigned int ALLOC = 1;

    int
rdx_pat_verify
    (
        PNODE *pnodep,
        VERIFY_MODE vm,
        FILE *fp
    )
{
    int tot_free_nodes;
    int tot_alloc_nodes;

    // branch node and data node free list head pointers
    BNODE *bhead;
    DNODE *dhead;

    // holds all of the allocated branch node addresses
    unsigned long bnode_addrs[NUM_KEYS][MAX_NUM_RDX_NODES+1];

    // holds all of the unallocated branch node addresses
    unsigned long free_bnode_addrs[NUM_KEYS][MAX_NUM_RDX_NODES+1];

    // holds all of the allocated data node addresses
    unsigned long dnode_addrs[MAX_NUM_RDX_NODES+1];

    // holds all of the unallocated data node addresses
    unsigned long free_dnode_addrs[MAX_NUM_RDX_NODES+1];

    // hold all of the allocated data node keys
    unsigned char dnode_keys[NUM_KEYS][MAX_NUM_RDX_NODES+1][NUM_KEY_BYTES];

    // holds integer indexes(0-MAX_NUM_RDX_NODES) of free and allocated nodes
    unsigned int node_index[2][MAX_NUM_RDX_NODES+1];


    /*
     * accumulate all the free and allocated branch and data node addresses, the node indexes on
     * each free/alloc node and the total number of free and allocated nodes
     */
    tot_free_nodes = 0;
    tot_alloc_nodes = 0;
    for ( int n = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
    {
        if ( pnodep->dnodes[n].alloc == 0 )
        {
            node_index[FREE][tot_free_nodes] = n;
            free_dnode_addrs[tot_free_nodes] = (unsigned long)&pnodep->dnodes[n];
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                free_bnode_addrs[k][tot_free_nodes] = (unsigned long)&pnodep->bnodes[n][k];
            }

            tot_free_nodes++;
        }
        else
        {
            node_index[ALLOC][tot_alloc_nodes] = n;
            dnode_addrs[tot_alloc_nodes] = (unsigned long)&pnodep->dnodes[n];
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                bnode_addrs[k][tot_alloc_nodes] = (unsigned long)&pnodep->bnodes[n][k];
                memmove( &dnode_keys[k][tot_alloc_nodes][0], &pnodep->dnodes[n].key[k][1], NUM_KEY_BYTES );
            }

            tot_alloc_nodes++;
        }
    }

    // print all of the free/alloc data structure addresses, node indexes and keys
    if ( vm == ERR_CODE_PRINT )
    {
        fprintf(fp, "Free Data Node Address(DNA)/Keys and Branch Node Addresses(BNAs)\n");
        fprintf(fp, "%-4s  ", "node");
        fprintf(fp, "%-20s", "DNA");
        fprintf(fp, "%10s", "BNA(s)  ");
        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            fprintf(fp, "key = %-14d", k);
        }
        fprintf(fp, "\n");

        for ( int n = 0 ; n < tot_free_nodes ; n++ )
        {
            fprintf(fp, "%4d  %-20p", node_index[FREE][n], (void *)free_dnode_addrs[n]);
            fprintf(fp, "%10s", " ");
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%-20p", (void *)free_bnode_addrs[k][n]);
            }
            fprintf(fp, "\n");
        }
        fprintf(fp, "\n");

        fprintf(fp, "Allocated Data Node Address(DNA)/Keys and Branch Node Addresses(BNAs)\n");
        fprintf(fp, "%-4s  ", "node");
        fprintf(fp, "%-20s", "DNA");
        fprintf(fp, "%10s", "BNA(s)  ");
        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            fprintf(fp, "key = %-14d", k);
        }
        fprintf(fp, "\n");

        for ( int n = 0 ; n < tot_alloc_nodes ; n++ )
        {
            fprintf(fp, "%4d  %-20p", node_index[ALLOC][n], (void *)dnode_addrs[n]);
            fprintf(fp, "%10s", " ");
            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "%-20p", (void *)bnode_addrs[k][n]);
            }
            fprintf(fp, "\n");

            for ( int k = 0 ; k < NUM_KEYS ; k++ )
            {
                fprintf(fp, "      key %2d = ", k);
                for ( int b = 0 ; b < NUM_KEY_BYTES ; b++ )
                {
                    fprintf(fp, "%02x ", dnode_keys[k][n][b]);
                }
                fprintf(fp, "\n");
            }
            fprintf(fp, "\n");
        }
        fprintf(fp, "\n");
    }

    /*
     * check if certain root node values are initialized to zero - there will
     * be NUM_KEYS branch nodes and one data node in the root node - these
     * values should remain permanently invariant at zero
     */
    for ( int k = 0 ; k < NUM_KEYS ; k++ )
    {
        if ( pnodep->bnodes[0][k].id != 0 )
        {
            if ( vm == ERR_CODE_PRINT )
            {
                fprintf(fp, "rdx_pat_verify():(file %s  line %d): Root branch node with non-zero id - %d\n",
                    __FILE__, __LINE__, pnodep->bnodes[0][k].id);
            }
            return 1;
        }

        if ( pnodep->bnodes[0][k].br != 0 )
        {
            if ( vm == ERR_CODE_PRINT )
            {
                fprintf(fp, "rdx_pat_verify():(file %s  line %d): Root branch node with non-zero br - %d\n",
                    __FILE__, __LINE__, pnodep->bnodes[0][k].br);
            }
            return 2;
        }

        if ( pnodep->bnodes[0][k].p != 0 )
        {
            if ( vm == ERR_CODE_PRINT )
            {
                fprintf(fp, "rdx_pat_verify():(file %s  line %d): Root branch node with non-zero p - %p\n",
                    __FILE__, __LINE__, (void *)pnodep->bnodes[0][k].p);
            }
            return 3;
        }

        if ( pnodep->bnodes[0][k].b != NUM_KEY_BYTES*8 )
        {
            if ( vm == ERR_CODE_PRINT )
            {
                fprintf(fp, "rdx_pat_verify():(file %s  line %d): Root branch node with b not equal to"
                            " NUM_KEY_BYTES*8 - %d\n",
                    __FILE__, __LINE__, pnodep->bnodes[0][k].b);
            }
            return 4;
        }

        if ( pnodep->bnodes[0][k].r != 0 )
        {
            if ( vm == ERR_CODE_PRINT )
            {
                fprintf(fp, "rdx_pat_verify():(file %s  line %d): Root branch node with non-zero r - %p\n",
                    __FILE__, __LINE__, (void *)pnodep->bnodes[0][k].r);
            }
            return 5;
        }

        for ( int b = 0 ; b < NUM_KEY_BYTES+1 ; b++ )
        {
            if ( pnodep->dnodes[0].key[k][b] != 0xff )
            {
                if ( vm == ERR_CODE_PRINT )
                {
                    fprintf(fp, "rdx_pat_verify():(file %s  line %d): Root data node keys not all 0xff\n",
                        __FILE__, __LINE__);
                }
                return 6;
            }
        }
    }

    /*
     * save data node, free node, branch node and key node address queues(will be unsorted),
     * total number of free nodes and total number of allocated nodes
     */
    for ( int n = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
    {
        if ( pnodep->dnodes[n].id != 1 )
        {
            if ( vm == ERR_CODE_PRINT )
            {
                fprintf(fp, "rdx_pat_verify():(file %s  line %d): Data node id not 1 in node sequence number %d\n",
                    __FILE__, __LINE__, n);
            }
            return 7;
        }

        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            if ( pnodep->bnodes[n][k].nsn != (unsigned) n )
            {
                if ( vm == ERR_CODE_PRINT )
                {
                    fprintf(fp, "rdx_pat_verify():(file %s  line %d): Branch node sequence number of key index %d"
                                " not valid - %d\n",
                        __FILE__, __LINE__, k, pnodep->bnodes[n][k].nsn);
                }
                return 8;
            }
        }

        if ( pnodep->dnodes[n].nsn != (unsigned) n )
        {
            if ( vm == ERR_CODE_PRINT )
            {
                fprintf(fp, "rdx_pat_verify():(file %s  line %d): Data node sequence number not valid - %d\n",
                    __FILE__, __LINE__, pnodep->dnodes[n].nsn);
            }
            return 9;
        }

        if ( pnodep->dnodes[n].alloc != 0 && pnodep->dnodes[n].alloc != 1 )
        {
            if ( vm == ERR_CODE_PRINT )
            {
                fprintf(fp, "rdx_pat_verify():(file %s  line %d): Data node allocated boolean not 0/1"
                            " in node sequence number %d\n",
                    __FILE__, __LINE__, pnodep->dnodes[n].nsn);
            }
            return 10;
        }
    }

    // verify that the sum of free and allocated nodes is MAX_NUM_RDX_NODES+1 nodes
    if ( (tot_alloc_nodes + tot_free_nodes) != MAX_NUM_RDX_NODES+1 )
    {
        if ( vm == ERR_CODE_PRINT )
        {
            fprintf(fp, "rdx_pat_verify():(file %s  line %d): Total allocated nodes(%d) + free nodes(%d)"
                        " not equal MAX_NUM_RDX_NODES+1(%d)\n",
                __FILE__, __LINE__, tot_alloc_nodes, tot_free_nodes, MAX_NUM_RDX_NODES+1);
        }
        return 11;
    }

    // verify that the tot_alloc_nodes equals tot_nodes
    if ( (unsigned) tot_alloc_nodes != (pnodep->tot_nodes+1) )
    {
        if ( vm == ERR_CODE_PRINT )
        {
            fprintf(fp, "rdx_pat_verify():(file %s  line %d): Total allocated nodes(%d)"
                        " not equal pnodep->tot_nodes(%d)\n",
                __FILE__, __LINE__, tot_alloc_nodes, pnodep->tot_nodes);
        }
        return 12;
    }

    // sort data node, free node, branch node and data node key address queues
    qsort(
             (void *)&dnode_addrs[0],
             tot_alloc_nodes,
             sizeof(unsigned long),
             unsigned_int_compare
         );

    qsort(
             (void *)&free_dnode_addrs[0],
             tot_free_nodes,
             sizeof(unsigned long),
             unsigned_int_compare
         );

    for ( int k = 0 ; k < NUM_KEYS ; k++ )
    {
        qsort(
                 (void *)&bnode_addrs[k][0],
                 tot_alloc_nodes,
                 sizeof(unsigned long),
                 unsigned_int_compare
             );

        qsort(
                 (void *)&free_bnode_addrs[k][0],
                 tot_free_nodes,
                 sizeof(unsigned long),
                 unsigned_int_compare
             );

        qsort(
                 (void *)&dnode_keys[k][0][0],
                 tot_alloc_nodes,
                 NUM_KEY_BYTES,
                 string_compare
             );
    }

    /*
     * check for multiple nodes with same key in a given key index - in sorted
     * order identical keys will show up next to each other
     */
    for ( int k = 0 ; k < NUM_KEYS ; k++ )
    {
        for ( int n = 0 ; n < tot_alloc_nodes-1 ; n++ )
        {
            if ( memcmp( &dnode_keys[k][n][0], &dnode_keys[k][n+1][0], NUM_KEY_BYTES ) == 0 )
            {
                if ( vm == ERR_CODE_PRINT )
                {
                    fprintf(fp, "rdx_pat_verify():(file %s  line %d): Multiple nodes with same key of key index %d"
                                " and key value\n   ",
                        __FILE__, __LINE__, k);
                    for ( int b = 0 ; b < NUM_KEY_BYTES ; b++ )
                    {
                        fprintf(fp, "%02x ", dnode_keys[k][n][b]);
                    }
                    fprintf(fp, "\n");
                }
                return 13;
            }
        }
    }

    /*
     * walk branch node free list(s) - checking for valid branch node addresses.
     * if head pointer is NULL then the free node list is empty
     */
    for ( int k = 0 ; k < NUM_KEYS ; k++ )
    {
        bhead = (BNODE *)pnodep->bfree_head[k];

        // walk data node free list
        while ( bhead != NULL )
        {
            void *ptr;

            // check if current pointer is in free data node list
            ptr = bsearch(
                             (void *)&bhead,
                             (void *)&free_bnode_addrs[k][0],
                             tot_free_nodes,
                             sizeof(unsigned long),
                             unsigned_int_compare
                         );
            if ( ptr == NULL )
            {
                if ( vm == ERR_CODE_PRINT )
                {
                    fprintf(fp, "rdx_pat_verify():(file %s  line %d): Pointer in branch node free list"
                                " with key index %d corrupted\n",
                        __FILE__, __LINE__, k);
                }
                return 14;
            }
            bhead = (BNODE *)bhead->p;
        }
    }

    /*
     * walk data node free list - checking for valid data node addresses.
     * if head pointer is NULL then the free node list is empty
     */
    dhead = (DNODE *)pnodep->dfree_head;

    // walk data node free list
    while ( dhead != NULL )
    {
        void *ptr;

        // check if current pointer is in free data node list
        ptr = bsearch(
                         (void *)&dhead,
                         (void *)&free_dnode_addrs[0],
                         tot_free_nodes,
                         sizeof(unsigned long),
                         unsigned_int_compare
                     );
        if ( ptr == NULL )
        {
            if ( vm == ERR_CODE_PRINT )
            {
                fprintf(fp, "rdx_pat_verify():(file %s  line %d): Pointer in data node free list corrupted\n",
                    __FILE__, __LINE__);
            }
            return 15;
        }
        dhead = (DNODE *)dhead->nnfp;
    }

    // for each allocated node check for valid branch and data node values
    for ( int n = 0 ; n < tot_alloc_nodes ; n++ )
    {
        unsigned long ui;
        void *ptr, *bptr, *dptr;


        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            // check if bad branch node id
            if ( pnodep->bnodes[n][k].id != 0 )
            {
                if ( vm == ERR_CODE_PRINT )
                {
                    fprintf(fp, "rdx_pat_verify():(file %s  line %d): Branch node id at key index %d and node %d not"
                                " valid - %d\n",
                        __FILE__, __LINE__, k, n, pnodep->bnodes[n][k].id);
                }
                return 16;
            }

            // check if bad parent branch indicator
            if ( pnodep->bnodes[n][k].br != 0 && pnodep->bnodes[n][k].br != 1 )
            {
                if ( vm == ERR_CODE_PRINT )
                {
                    fprintf(fp, "rdx_pat_verify():(file %s  line %d): Branch node parent branch boolean of key index %d"
                                " and node %d not valid - %d\n",
                        __FILE__, __LINE__, k, n, pnodep->bnodes[n][k].br);
                }
                return 17;
            }

            // check if branch node parent pointers are valid - the root node(i=0) will not have a parent
            if ( n != 0 )
            {
                ui = (unsigned long)pnodep->bnodes[n][k].p;
                ptr = bsearch(
                                 (void *)&ui,
                                 (void *)&bnode_addrs[k][0],
                                 tot_alloc_nodes,
                                 sizeof(unsigned long),
                                 unsigned_int_compare
                             );
                if ( ptr == NULL )
                {
                    if ( vm == ERR_CODE_PRINT )
                    {
                        fprintf(fp, "rdx_pat_verify():(file %s  line %d): Branch node parent pointer of key index %d"
                                    " and node %d corrupted\n",
                            __FILE__, __LINE__, k, n);
                    }
                    return 18;
                }
            }

            // check if PATRICIA algorithm branch test bit number is valid
            if ( pnodep->bnodes[n][k].b > NUM_KEY_BYTES*8 )
            {
                if ( vm == ERR_CODE_PRINT )
                {
                    fprintf(fp, "rdx_pat_verify():(file %s  line %d): Branch node bit number branch test value of"
                                " key index %d and node %d not valid - %d\n",
                        __FILE__, __LINE__, k, n, pnodep->bnodes[n][k].b);
                }
                return 19;
            }

            // check if left child pointer is another branch or data node
            ui = (unsigned long)pnodep->bnodes[n][k].l;
            bptr = bsearch(
                              (void *)&ui,
                              (void *)&bnode_addrs[k][0],
                              tot_alloc_nodes,
                              sizeof(unsigned long),
                              unsigned_int_compare
                          );
            dptr = bsearch(
                              (void *)&ui,
                              (void *)&dnode_addrs[0],
                              tot_alloc_nodes,
                              sizeof(unsigned long),
                              unsigned_int_compare
                          );
            if ( bptr == NULL && dptr == NULL )
            {
                if ( vm == ERR_CODE_PRINT )
                {
                    fprintf(fp, "rdx_pat_verify():(file %s  line %d): Branch node left pointer of key index %d"
                                " and node %d corrupted\n",
                        __FILE__, __LINE__, k, n);
                }
                return 20;
            }

            /*
             * check if right child pointer is another branch or data node
             * - the root node(i=0) will not have a right child
             */
            if ( n != 0 )
            {
                ui = (unsigned long)pnodep->bnodes[n][k].r;
                bptr = bsearch(
                                  (void *)&ui,
                                  (void *)&bnode_addrs[k][0],
                                  tot_alloc_nodes,
                                  sizeof(unsigned long),
                                  unsigned_int_compare
                              );
                dptr = bsearch(
                                  (void *)&ui,
                                  (void *)&dnode_addrs[0],
                                  tot_alloc_nodes,
                                  sizeof(unsigned long),
                                  unsigned_int_compare
                              );
                if ( bptr == NULL && dptr == NULL )
                {
                    if ( vm == ERR_CODE_PRINT )
                    {
                        fprintf(fp, "rdx_pat_verify():(file %s  line %d): Branch node right pointer of key index %d"
                                    " and node %d corrupted\n",
                            __FILE__, __LINE__, k, n);
                    }
                    return 21;
                }
            }

            // check if data node parent pointers are all valid branch node addresses
            ui = (unsigned long)pnodep->dnodes[n].p[k];
            ptr = bsearch(
                             (void *)&ui,
                             (void *)&bnode_addrs[k][0],
                             tot_alloc_nodes,
                             sizeof(unsigned long),
                             unsigned_int_compare
                         );
            if ( ptr == NULL )
            {
                if ( vm == ERR_CODE_PRINT )
                {
                    fprintf(fp, "rdx_pat_verify():(file %s  line %d): Data node parent pointer of key index %d"
                                " and node %d corrupted\n",
                        __FILE__, __LINE__, k, n);
                }
                return 22;
            }

            // check if all data node parent branch indicators are valid
            if ( pnodep->dnodes[n].br[k] != 0 && pnodep->dnodes[n].br[k] != 1 )
            {
                if ( vm == ERR_CODE_PRINT )
                {
                    fprintf(fp, "rdx_pat_verify():(file %s  line %d): Data node parent branch boolean of key index %d and"
                                " node %d not valid - %d\n",
                        __FILE__, __LINE__, k, n, pnodep->dnodes[n].br[k]);
                }
                return 23;
            }
        }
    }

    /*
     * retrieve all keys from all data nodes and use normal search(rdx_pat_search())
     * to find them - ignore root node keys
     */
    for ( int n = 1 ; n < tot_alloc_nodes ; n++ )
    {
        unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES];
        DNODE *dnodep;


        // get data node keys directly from allocated data node
        for ( int k = 0 ; k < NUM_KEYS ; k++ )
        {
            key[k][0] = 1;
            memmove( &key[k][1], &pnodep->dnodes[n].key[k][1], NUM_KEY_BYTES );
        }

        // search for node n keys
        dnodep = rdx_pat_search(pnodep, key);

        if ( dnodep == NULL )
        {
            if ( vm == ERR_CODE_PRINT )
            {
                fprintf(fp, "rdx_pat_verify():(file %s  line %d): Data node %d searched for with it's keys not found\n",
                    __FILE__, __LINE__, n);
            }
            return 24;
        }

        /*
         * do full data node memory compare of node i and node found with keys
         * from node i - if they are not the same then error
         */
        if ( memcmp( dnodep, &pnodep->dnodes[n], sizeof(DNODE) ) != 0 )
        {
            if ( vm == ERR_CODE_PRINT )
            {
                fprintf(fp, "rdx_pat_verify():(file %s  line %d): Data node at index %d and data node found by search"
                            " with index %d keys are not the same\n",
                    __FILE__, __LINE__, n, n);
            }
            return 25;
        }
    }

    return 0;
}

