```
rdx
===

Multi Key Radix PATRICIA Fast Search C/C++/Clang
   - in which "PATRICIA--Practical Algorithm To Retrieve Information Coded in Alphanumeric," by Morrison, Donald R.
     ACM, Vol. 15, No. 4, Oct. 1968. is used to store data in a data structure of N keys of arbitrary length and
     arbitrary number of data nodes
   - each data node requires exactly N branch nodes, thus the entire data structure may be pre-allocated
   - see article in Embedded Systems Programming, Mar. 1997, "Highly dynamic Radix fast search algorithm with easy
     sort/deletion"(included)
   - see article in Embedded Systems Design, Jun. 2007, "Using multikey radix PATRICIA fast search"(included)
   - 15 verification tests that show complex code usage examples, see rdx_pat_test.c
   - test code is run with three sized tries:
         1. 8 nodes, 3 keys, 4 bytes per key
         2. 1 nodes, 1 keys, 1 bytes per key
         3. 500 nodes, 5 keys, 12 bytes per key

   DESIGN GOALS:
   1. Flexibility   - The specification of three parameters - MAX_NUM_RDX_NODES, NUM_KEYS and NUM_KEY_BYTES - allows
                      the construction of a very wide range of PATRICIA tries without having to resort to reinvention
                      of code to provide some spcific combination of these parameters.

   2. Originality   - The property of PATRICIA that each new key insertion results in one new branch node allocation
                      allows for the allocation of a data node and NUM_KEYS branch nodes for each new multi-key
                      insertion.  This makes the full allocation of trie space at the outset possible and the
                      insertion and deletion of data node entirely a matter of pointer manipulation.  I know of
                      no other implementation using this approach.

   3. Verifyability - With the use of the two routines, rdx_pat_print() and rdx_pat_verify(), the structural details
                      of the tries may be checked.  rdx_pat_print() can print out the entire trie structure including
                      pointers.  With a specific key argument the NUM_KEYS branch node paths to the data is printed.
                      The rdx_pat_verify() routine checks for twenty five types of trie structural mishaps.

   4. Testing       - Three tests of different sized tries are provided.  The test code is identical in each case.
                      These are:
                          a. 8 nodes, 3 keys, 4 bytes per key
                          b. 1 nodes, 1 keys, 1 bytes per key
                          c. 500 nodes, 5 keys, 12 bytes per key

                      The a. trie has fifteen tests.  All tests are output to the rdx_pat_test1.results file.
                      Each test output is delineated from other tests and documented as to what result is expected.
                      Two tries are declared and used for the tests.  However, since the rdx_pat_search.h file is
                      compiled in the tries must be of the same size.

                      The b. trie is the minimum specifiable.  No one would actually use a trie of one node with
                      one key of one byte, however, this minimal configuration should still work.  Some tests will
                      not be run since they require exactly three keys.

                      The c. trie is a much larger trie test.  Again, some tests will not be run since they require
                      exactly three keys.

   - Latest Version v1.0.4:
         a. fixed bug related to search/delete for a subset(1 to NUM_KEYS-1) of keys.  a byproduct of this
            fix is faster execution for both search/delete - fewer array accesses.
         b. the code is now compiled with -std=c11.  this required no changes, but some changes were
            made that proved useful e.g. for (int=0 ;;) and %p.
         c. the code for rdx_pat_print() and rdx_pat_verify() has been heavily updated to improve the
            print outs.
         d. all code, rdx_pat_search.c and test code, has been updated with many minor doc and formatting changes.
         e. the test code now comes in three versions.  All tests are the same, however, the trie is different
            in each case:
                1. 8 nodes, 3 keys, 4 bytes per key
                2. 1 nodes, 1 keys, 1 bytes per key
                3. 500 nodes, 5 keys, 12 bytes per key
            number 1 is the same as I have been using before.
            number 2 is the limiting case of 1 node with 1 key with 1 byte.
            number 3 is a test with a much larger trie.  this produces a 4M output file.
            running './rdx_pat.mk' will run all the tests.  the code and output is numbered by 1,2,3.
         f. the rdx_pat.mk script now has a clean mode.  do './rdx_pat.mk clean'.

   - Version v1.0.3:
         a. modified insert to use the same type keys array with key boolean bytes required to be set to 1
            (use all keys).  this makes the the keys array arguments uniform over all routines.
         b. updated and simplified the test code the reflect the change to the insert routine.
         c. ran tests with much larger MAX_NUM_RDX_NODES, NUM_KEYS and NUM_KEY_BYTES.

   - Version v1.0.2:
         a. search/delete/print now only require a subset of keys(from 1 to NUM_KEYS keys) - insert still requires
            a full set of keys of course.
         b. the distinction between C and C++ code is removed.  only one version is provided.  the rdx_pat.mk
            script can be run with clang, gcc or g++.
         c. the test suite has been improved substantially.  new tests for search/delete/print with a single key
            have been added.  the rdx_pat_test.results file of test output should be much more readable and the
            correspondence of results with example code should be clearer.

API:

/*
 *======================================================================================================================
 *
 * File Names:
 *     rdx_pat_search.c
 *     rdx_pat_search.h
 *     rdx_pat_data.h
 *
 *======================================================================================================================
 *
 * Prototypes:
 *
 *         void
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
 *     rdx_pat_delete
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
 *     in the APP_DATA typedef in rdx_pat_data.h and then specify in rdx_pat_search.h the values of the three defines
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
 *
 *     rdx_pat_initialize(&data);
 *
 * Returns:
 *     void
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

/*
 *======================================================================================================================
 *     rdx_pat_delete()
 *
 * Purpose: 
 *     delete trie data node with the keys key[NUM_KEYS][1+NUM_KEY_BYTES].  the second subscript first byte for all
 *     keys(key[k][0]) is the key boolean.  the key boolean should be set to 1(use the key) or 0(do not use key).
 *     the actual key bytes(key[k][1 to NUM_KEY_BYTES]) follow the key boolean.  thus, 1 to NUM_KEYS keys may be used.
 *     only one key is necessary to delete the data node.  each key must be unique within it's key
 *     index(0 to NUM_KEYS-1).
 *
 * Usage:
 *     PNODE data;
 *     unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES];
 *     DNODE *dnp;
 *
 *     dnp = rdx_pat_delete(&data, key);
 *
 * Returns:
 *     DNODE *dnp - pointer to the data node deleted or NULL if no data node found
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

```
