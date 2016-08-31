```

rdx
===

 Multi-key Radix PATRICIA Fast Search - OO C++ - A header only class(MKRdxPat.hpp)

     - The MKRdxPat class allows the allocation of a fixed sized contiguous data store that holds data nodes
       of an arbitrary structure that may be accessed with any number of keys of any size with the PATRICIA
       (Practical Algorithm To Retrieve Information Coded In Alphanumeric)(1,2,3) fast search algorithm.
       MKRdxPat is particularly suited to applications that require complex data structures be contiguously
       stored and accessed with an algorithm of known fast character with any of several possible keys,
       either singly of several simultaneously.  MKRdxPat is unique, as far as I know, in allowing search
       with multiple keys of any length.  If the application only requires a single key, this is OK; just
       set num_keys to 1 in the constructor call.  See below for the full documentation.

     - 15 cut/paste(MKRdxPat_test.cpp) verification tests that show code usage examples

     - no library dependencies

     - DESIGN GOALS:
           1. Flexibility
           2. Originality
           3. Verifiability
           4. Testing
           5. Performance

     - see article in Embedded Systems Programming, Mar. 1997, "Highly dynamic Radix fast search
       algorithm with easy sort/deletion"(included)

     - see article in Embedded Systems Design, Jun. 2007, "Using multi-key radix PATRICIA fast
       search"(included)

     - performance measurement executable
           e.g.
               PERFORMANCE TEST: Do repeated rdx->insert()(fill trie) / rdx->remove()(empty trie)
                                 monatonic keys
               Model name:            AMD Athlon(tm) 64 X2 Dual Core Processor 4000+
               CPU MHz:               2109.535

               max_rdx_nodes = 2,000,000
               num_keys = 3
               max_key_bytes = 16

               trie size = 558,000,327b

               seconds = 1131.294222  total inserts/removes = 400,000,000

     - ACKNOWLEDGMENT
         Pavel Odintsov(pavel.odintsov@gmail.com)

         Pavel provided the file of BGP routing prefixes from one of his BGP speakers
         for the MKRdxPat_perf_bgp performance test program

 Do:

 1. cd rdx/cpp

 2. a. vi ./MKRdxPat.hpp for the full documentation with examples in the comments at the
       beginning of the file.  also, replicated below.
    b. the library is not dependent on any other libraries.
    c. the MKRdxPat_test.cpp executable has an extensive set of examples that can easily
       be cut and paste into your application.
    d. the MKRdxPat_perf.cpp executable measures performance.  the comments at the
       beginning of the file gives details.
    e. the MKRdxPat_perf_bgp.cpp executable measures performance with real BGP data.
       the comments at the beginning of the file gives details.

 3. adjust $CC in ./MKRdxPat.mk to use clang or g++

 4. do "./MKRdxPat.mk" to build and run the functional tests and compile the performance
    executables.

 5. do "./MKRdxPat.mk clean" to clean the directory

 6. vi MKRdxPat_perf.cpp and examine the comments at the beginning of the file to learn
    how to run the performance test.  to modify the performance tests to better fit your
    application edit MKRdxPat_perf.cpp and install your application data in
    'struct app_data{}' and set the MKRdxPat.hpp class constructor arguments:

    //
    // set these application specific data
    //
    // ================================================

    // MKRdxPat.hpp class constructor arguments
    const int max_rdx_nodes = 200000;
    const int num_keys      = 2;
    const int max_key_bytes = 16;

    // application data of type app_data defined here
    struct app_data
    {
        int i;
    };

    // ================================================


    do "./MKRdxPat.mk" to build

    do "./MKRdxPat_perf [options]" to run performance tests with
    your application specific data

 7. vi MKRdxPat_perf_bgp.cpp and examine the comments at the beginning of the file to learn
    how to run the BGP performance test.  to modify the BGP performance test to better fit
    your application edit MKRdxPat_perf_bgp.cpp and install your application data in
    'struct app_data{}'.  unpack BGP_routing_table_3peersnap_04102016_as198068.xz:

    //
    // change only struct app_data {}; if desired and nothing else
    //
    // ================================================

    // MKRdxPat.hpp class constructor arguments
    const int max_rdx_nodes = BGP_SIZE;  // fixed - routing table entries
    const int num_keys      = 1;  // fixed - one key(prefix) in routing table

    // store keys in trie as:\n"
    //     1 - d(3).d(3).d(3).d(3)/n(2) ascii(up to 18 bytes)
    //     2 - d(3)d(3)d(3)d(3)n(2) ascii(up to 14 bytes)
    //     3 - (32 bit uint)(4)n(1) binary(5 bytes)
    const int max_key_bytes = MAX_PREFIX_SIZE;

    // application data of type app_data defined here
    struct app_data
    {
        int i;
    };

    // ================================================

    do "./MKRdxPat.mk" to build

    do "./MKRdxPat_perf_bgp [options]" to run bgp performance tests with
    your application specific data


 *======================================================================================================================
 *
 * File Names:
 *     MKRdxPat.hpp
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
 *             int MAX_RDX_NODES,
 *             int NUM_KEYS,
 *             int MAX_KEY_BYTES
 *         )
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
 *             const unsigned char *key,  // unsigned char key[NUM_KEYS][1+MAX_KEY_BYTES]
 *             app_data **app_datapp
 *         )
 *         e.g. int return_code = rdx->insert((unsigned char *)key, &app_datap);
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
 *             const unsigned char *key  // unsigned char key[NUM_KEYS][1+MAX_KEY_BYTES]
 *         )
 *         e.g. app_data *app_datap = rdx->search((unsigned char *)key);
 *
 *
 *     struct app_data
 *     {
 *         int data;  // user specified application data structure
 *     };
 *
 *         app_data *
 *     remove
 *         (
 *             const unsigned char *key  // unsigned char key[NUM_KEYS][1+MAX_KEY_BYTES]
 *         )
 *         e.g. app_data *app_datap = rdx->remove((unsigned char *)key);
 *
 *
 *         int
 *     sort
 *         (
 *             app_data ***app_datappp,
 *             const int k
 *         )
 *         e.g. int return_code = rdx->sort(&app_datapp, k);
 *
 *
 *         int
 *     keys
 *         (
 *             unsigned char *key  // unsigned char key[NUM_KEYS][1+MAX_KEY_BYTES]
 *         )
 *         e.g. int return_code = rdx->keys((unsigned char *)key);
 *
 *
 *         int
 *     alloc_nodes
 *         (
 *         ) const
 *         e.g. int alloc_nodes = rdx->alloc_nodes();
 *
 *
 *         int
 *     bsize
 *         (
 *         ) const
 *         e.g. int bsize = rdx->bsize();
 *
 *
 *         int
 *     max_rdx_nodes
 *         (
 *         ) const
 *         e.g. int max_rdx_nodes = rdx->max_rdx_nodes();
 *
 *
 *         int
 *     num_keys
 *         (
 *         ) const
 *         e.g. int num_keys = rdx->num_keys();
 *
 *
 *         int
 *     max_key_bytes 
 *         (
 *         ) const
 *         e.g. int max_key_bytes = rdx->max_key_bytes();
 *
 *
 *         MKRdxPat<app_data> *
 *     chg_max_rdx_nodes 
 *         (
 *             const unsigned int new_max_rdx_nodes
 *         ) const
 *         e.g. MKRdxPat<app_data> *rdx_new = rdx_old->chg_max_rdx_nodes(new_max_rdx_nodes);
 *
 *
 *         int
 *     print
 *         (
 *             const unsigned char *key,  // unsigned char key[NUM_KEYS][1+MAX_KEY_BYTES]
 *             ofstream& os
 *         )
 *         e.g. int return_code = rdx->print(NULL, os);
 *         e.g. int return_code = rdx->print((unsigned char *)key, os);
 *
 *
 *         int
 *     verify
 *         (
 *             VERIFY_MODE vm,
 *             ofstream& os
 *         )
 *         e.g. int return_code = rdx->verify(ERR_CODE, os);
 *         e.g. int return_code = rdx->verify(ERR_CODE_PRINT, os);
 *
 *======================================================================================================================
 *
 * Description:
 *     The MKRdxPat class allows the allocation of a fixed sized contiguous data store that holds data nodes
 *     of an arbitrary structure that may be accessed with any number of keys of any size with the PATRICIA
 *     (Practical Algorithm To Retrieve Information Coded In Alphanumeric)(1,2,3) fast search algorithm.
 *     MKRdxPat is particularly suited to applications that require complex data structures be contiguously
 *     stored and accessed with an algorithm of known fast character with any of several possible keys,
 *     either singly of several simultaneously.
 *
 *     For example, a data structure that required either of an IPv4, IPv6 or MAC address key(s) to access
 *     data nodes.  In this case NUM_KEYS would be 3 and MAX_KEY_BYTES would be 16(the longest - see below).
 *
 *     The MKRdxPat class supports a data structure of MAX_RDX_NODES data nodes and NUM_KEYS keys per
 *     data node with keys of MAX_KEY_BYTES bytes or less.  The class constructor:
 *
 *         MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_RDX_NODES, NUM_KEYS, MAX_KEY_BYTES);
 *
 *     along with a:
 *
 *         struct app_data
 *         {
 *             // user data
 *         };
 *
 *     class constructor typename specifies the data structure.  A data node may be accessed with a single
 *     key or any number of keys simultaneously.  Keys must be unique within their key index(0 - NUM_KEYS-1),
 *     but not over different key indexes.  Member functions that require keys are passed the array:
 *
 *         unsigned char key[NUM_KEYS][1+MAX_KEY_BYTES]
 *
 *     One extra prefix byte, a key boolean - required to be 0(ignore) or 1(search), is set with each key to
 *     specify if the key is to be used in the data node search.  It is suggested that the array be first
 *     memset() to 0:
 *
 *         memset(key, 0, NUM_KEYS * (1+MAX_KEY_BYTES));
 *
 *     and then only keys to be used copied in with the key boolean(set to 1) prepended.
 *
 *     In most cases the necessary keys will be of different lengths.  The MAX_KEY_BYTES constructor argument
 *     would be set to the longest of these.  All other shorter keys would be left justified in the key[][]
 *     array, that is, start just after the key boolean.  Thus, for the example above, the IPv6 key would use
 *     the full 16 bytes, the IPv4 key would use the first four bytes after the key boolean(with the rest(12)
 *     set to 0) and the MAC key would use the first 6 bytes after the key boolean(with the rest(10) set to
 *     0).  This was done to make the specification of the key[][] array and the code associated with
 *     processing it simpler.  Actually, the IPv4 and MAC addresses could be right justified, just so long as
 *     all the keys are always right justified and the other bytes are 0; you just have to be consistent
 *     within any given key index.  From the way that PATRICIA works the keys are examined from left to
 *     right.  Thus, it makes sense to left justify.  Then, the actual key bits are examined instead of
 *     examining irrelevent 0 bits first.  The suffixed 0 bits are not a factor in the data node search since
 *     the key bits on the left will find a data node if all inserts were made with the same number of key
 *     bits on the left e.g. 32 for an IPv4 address.  Example:
 *
 *                 kb key bytes
 *         IPv4 :  01 c0 a8 00 01 00 00 00 00 00 00 00 00 00 00 00 00 - 192.168.0.1
 *         IPv6 :  01 fe 80 00 00 00 00 00 00 02 21 2f ff fe b5 6e 10 - fe80::221:2fff:feb5:6e10
 *         MAC  :  01 00 21 2f b5 6e 10 00 00 00 00 00 00 00 00 00 00 - 00:21:2f:b5:6e:10
 *
 *     Let's suppose that we wanted to find a data node with just the IPv6 address.  We first memset()
 *     key[][] to 0.  We then add the IPv6 address with it's key boolean set to 1 and pass the key[][] array
 *     to the member functions.  Thus:
 *
 *                 kb key bytes
 *         IPv4 :  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 - 0
 *         IPv6 :  01 fe 80 00 00 00 00 00 00 02 21 2f ff fe b5 6e 10 - fe80::221:2fff:feb5:6e10
 *         MAC  :  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 - 0
 *
 *     See the member function summary above or a detailed member function usage comment before each
 *     function's code.  A verify() member function is provided that does extensive data structure memory
 *     analysis and a print() member function is provided that prints the structural details of all the
 *     branch and data nodes.
 *
 *     For a trie of NUM_KEYS keys, each data node will have NUM_KEYS branch nodes associated with it.  The
 *     number of actual data nodes in the data structure is MAX_RDX_NODES+1.  This extra node is for the
 *     initial branch nodes and initial data node with the impossible key of all 0xff's(1,2,3).  The number
 *     of user storable nodes is MAX_RDX_NODES.   The user, through the constructor, declares PNODEs,
 *     each of which contains a radix PATRICIA trie of MAX_RDX_NODES nodes with NUM_KEYS keys of
 *     at most MAX_KEY_BYTES bytes length.
 *
 *     These routines are a modification of the algorithm cited.  Specifically, the upward pointers used to
 *     terminate search are not used, and are instead used to point to data nodes as trie leaves.  In
 *     multi-key search with PATRICIA the upward pointer scheme will not work since each key would have
 *     pointed to a different branch node with it's associated data structure.  In multi-key PATRICIA search
 *     all keys must lead to the same data node.  The viability of multi-key radix PATRICIA search depends
 *     on the fact that each data node insertion requires only one new branch node for each key, thus the
 *     initial allocation of NUM_KEYS branch nodes with each data node.  This implementation has several
 *     additional branch and data node members not used in the original PATRICIA search algorithm.  These
 *     are for various purposes; related to changes in the original algorithm for multi-key search, related
 *     to changes in the original algorithm for leaf data nodes instead of upward pointers to data structures
 *     stored in branch nodes, related to printing, related to sorting and debugging.
 *
 *======================================================================================================================
 *
 * Error Handling
 *
 *     1. Exceptions - The #include <exception> header is used to define exception objects throw()'n
 *                     upon calloc() failures.  Calloc() failures are thrown in the class constructor
 *                     MKRdxPat(), the chg_max_rdx_nodes() member function and the verify() member
 *                     function.  All other errors are reported via return values.
 *     2. Debugging  - Debugging output is provided for the class constructor - MKRdxPat() and member
 *                     functions insert(), search() and remove().  A debug() macro is provided.  It
 *                     will generte output if any one of three defines - DEBUG_I(insert()), DEBUG_S(search()),
 *                     DEBUG_R(remove()) - are set to 1.  If any of these are set to 1 then debug output for
 *                     MKRdxPat() will happen.
 *     3. Returns    - Member functions can return error indications.  These are documented in the comment
 *                     headings of the functions.  These are much faster than exceptions and should be
 *                     handled locally.
 *
 *======================================================================================================================
 *
 * Operational Notes:
 *
 *     1. the first member of BNODE and DNODE typedefs, id, should not be moved.  when doing operations on the
 *        trie, a search will reference the id from a BNODE type initially and then terminate at a DNODE type.
 *
 *     2. the maximum number of key bits is MAX_KEY_BITS.  the impossible key in the initially allocated data
 *        node must be at least one bit longer.  this requires the data node storage for the keys to have one
 *        extra byte.
 *
 *     3. all keys are the same length - MAX_KEY_BYTES bytes.  the performance hit, the small memory savings
 *        and the added code complexity of having individually sized keys did not seem worth it.  thus, the
 *        longest required key will determine MAX_KEY_BYTES.  shorter keys may be left justified in the
 *        MAX_KEY_BYTES bytes.
 *
 *     4. it is suggested that the 'unsigned char key[NUM_KEYS][1+MAX_KEY_BYTES]' array be first memset() to
 *        0 and then all the keys to be used with key boolean 1 added in one operation(keys with key boolean
 *        0 are never needed) e.g. memset(key, 0, NUM_KEYS * (1+MAX_KEY_BYTES));.
 *
 *     5. multidemensional arrays in C++ are implemented by subscripting calloc()'ed memory with parameterized
 *        values calculated from the C array versions.  One, two and three dimensional arrays are illustrated
 *        as follows(see verify() for actual usage):
 *
 *        n -> subscripts max_rdx_nodes_
 *        k -> subscripts num_keys_
 *        b -> subscripts max_key_bytes_:
 *
 *        a. one dimensional:
 *           unsigned long verify_dnode_addrs_[max_rdx_nodes_+1];
 *               -> verify_dnode_addrs_[n]
 *
 *        b. two dimensional:
 *           unsigned long verify_bnode_addrs_[num_keys_][max_rdx_nodes_+1];
 *               -> verify_bnode_addrs_[k*(max_rdx_nodes_+1)+n]
 *
 *        c. three dimensional:
 *           unsigned char verify_dnode_keys_[num_keys_][max_rdx_nodes_+1][max_key_bytes_];
 *               -> verify_dnode_keys_[(k*(max_rdx_nodes_+1)+n)*max_key_bytes_+b]
 *
 *======================================================================================================================
 *
 * Theoretic Notes:
 *
 *     1. depending on the key structure it is possible for the trie to be very unbalanced.  for a three byte
 *        key, for example, if 24 keys are inserted, each with only one different bit set, then the resulting
 *        trie would be a string of branch nodes extending to the left, each with a data node on its right
 *        branch.  the maximum trie depth would be 24 with the key of all zeros.
 *
 *     2. for the N node trie with b bit keys, even for very long keys, the average search requires log2(N)
 *        bit comparisons, and the average trie depth is log2(N).  see reference 1.  for a full key space,
 *        N = 2**b, log2(2**b) = b, and the average trie depth approaches b.
 *
 *     3. note that insertion requires two traversals of the trie - one to find if the key(s) already exists
 *        and the other to do the actual insertion.  the user may think that there must be a way to traverse
 *        down the trie only once to do an insertion.  the answer is YES! But.  in order to know where to
 *        stop you need to know all the key bits that intervene between the two b bit parameters of each
 *        branch node.  thus you would have to store these bits in each branch node or you could store any
 *        full key from any data node below a branch node in that branch node, which, by the property of a
 *        PATRICIA trie, will all have prefix bits identical for all data nodes below that branch node.  if,
 *        on insertion, the insertion key had bits differing between the two b bit parameters of two branch
 *        nodes then an insertion would be done at that point.  all this key storage and bit comparison in
 *        each traversed branch node is inefficient.  the first search traversal finds the key that does not
 *        match, but this key has all the key bits needed to find the first key bit of the new key that is
 *        different from the nearest key already in the trie.  this first differing bit then tells the second
 *        traversal where to stop and insert the new key.  thus, dual traversal is more efficient and simpler.
 *
 *======================================================================================================================
 *
 * References:
 *
 *     1. Algorithms in C++ Third Edition Parts 1-4, Robert Sedgewick with Christopher J. Van Wyk,
 *        Addison-Wesley, 1998, Chapter 15.3 Patricia Tries, pp. 637.
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

```
