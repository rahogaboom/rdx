```

rdx
===

 Multi-key Radix PATRICIA Fast Search C/OO C++

     - C and OO C++ versions

     - Boost Software License - Version 1.0 - August 17th, 2003

     - allows the allocation of a fixed sized contiguous data store that holds data nodes of an
       arbitrary structure that may be accessed with any number of keys of any size with the
       PATRICIA (Practical Algorithm To Retrieve Information Coded In Alphanumeric)(1,2,3) fast
       search algorithm

     - see article in Embedded Systems Programming, Mar. 1997, "Highly dynamic Radix fast search
       algorithm with easy sort/deletion"(included)

     - see article in Embedded Systems Design, Jun. 2007, "Using multi-key radix PATRICIA fast
       search"(included)

     - many verification tests that show complex code usage examples

     - performance measurement executable

     - DESIGN GOALS:
           1. Flexibility
           2. Originality
           3. Verifiability
           4. Testing
           5. Performance

 OO C++ version:

======================================================================================================================
 
 File Names:
     MKRdxPat.hpp

======================================================================================================================

 Member Functions:

     struct app_data
     {
         int data;  // user specified application data structure
     };

     MKRdxPat<app_data>
         (
             int MAX_NUM_RDX_NODES,
             int NUM_KEYS,
             int NUM_KEY_BYTES
         );
         e.g. MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(512, 3, 4);


     ~MKRdxPat()
         e.g. delete rdx;


     struct app_data
     {
         int data;  // user specified application data structure
     };

         int
     insert
         (
             unsigned char *key,  // unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
             app_data **app_datapp
         );
         e.g. return_code = rdx->insert((unsigned char *)key, &app_datap);


     struct app_data
     {
         int data;  // user specified application data structure
     };

         app_data *
     search
         (
             unsigned char *key  // unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
         );
         e.g. app_datap = rdx->search((unsigned char *)key);


     struct app_data
     {
         int data;  // user specified application data structure
     }

         app_data *
     remove
         (
             unsigned char *key  // unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
         );
         e.g. app_datap = rdx->remove((unsigned char *)key);


         int
     sort
         (
             app_data ***app_datappp,
             int k
         );
         e.g. return_code = rdx->sort(&app_datapp, k);


         int
     nodes
         (
         );
         e.g. nodes = rdx->nodes();


         int
     size 
         (
         );
         e.g. size = rdx->size();


         int
     print
         (
             unsigned char *key,  // unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
             ofstream& os
         );
         e.g. return_code = rdx->print(NULL, os);
         e.g. return_code = rdx->print((unsigned char *)key, os);


         int
     verify
         (
             VERIFY_MODE vm,
             ofstream& os
         );
         e.g. return_code = rdx->verify(ERR_CODE, os);
         e.g. return_code = rdx->verify(ERR_CODE_PRINT, os);

======================================================================================================================

 Description:
     The MKRdxPat class allows the allocation of a fixed sized contiguous data store that holds data nodes
     of an arbitrary structure that may be accessed with any number of keys of any size with the PATRICIA
     (Practical Algorithm To Retrieve Information Coded In Alphanumeric)(1,2,3) fast search algorithm.
     MKRdxPat is particularly suited to applications the require complex data structures be contiguously
     stored and accessed with an algorithm of known fast character with any of several possible keys,
     either singly of several simultaneously.

     For example, a data structure that required either of an IPv4, IPv6 or MAC address key(s) to access
     data nodes.  In this case NUM_KEYS would be 3 and NUM_KEY_BYTES would be 16(the longest - see below).

     The MKRdxPat class supports a data structure of MAX_NUM_RDX_NODES data nodes and NUM_KEYS keys per
     data node with keys of NUM_KEY_BYTES bytes.  The class constructor:

         MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);

     along with a:

         struct app_data
         {
             // user data
         };

     class constructor typename specifies the data structure.  A data node may be accessed with a single
     key or any number of keys simultaneously.  Keys must be unique within their key index(0 - NUM_KEYS-1),
     but not over different key indexes.  Member functions that require keys are passed the array:

         unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]

     One extra prefix byte, a key boolean - required to be 0(ignore) or 1(search), is set with each key to
     specify if the key is to be used in the data node search.  It is suggested that the array be first
     memset() to 0:

         memset(key, 0, NUM_KEYS * (1+NUM_KEY_BYTES));

     and then only keys to be used copied in with the key boolean(set to 1) prepended.

     In most cases the necessary keys will be of different lengths.  The NUM_KEY_BYTES constructor argument
     would be set to the longest of these.  All other shorter keys would be left justified in the key[][]
     array, that is, start just after the key boolean.  Thus, for the example above, the IPv6 key would use
     the full 16 bytes, the IPv4 key would use the first four bytes after the key boolean(with the rest(12)
     set to 0) and the MAC key would use the first 6 bytes after the key boolean(with the rest(10) set to
     0).  This was done to make the specification of the key[][] array and the code associated with
     processing it simpler.  Actually, the IPv4 and MAC addresses could be right justified, just so long as
     all the keys are always right justified and the other bytes are 0; you just have to be consistent
     within any given key index.  From the way that PATRICIA works the keys are examined from left to
     right.  Thus, it makes sense to left justify.  Then, the actual key bits are examined instead of
     examining irrelevent 0 bits first.  The suffixed 0 bits are not a factor in the data node search since
     the key bits on the left will find a data node if all inserts were made with the same number of key
     bits on the left e.g. 32 for an IPv4 address.  Example:

                 kb key bytes
         IPv4 :  01 c0 a8 00 01 00 00 00 00 00 00 00 00 00 00 00 00 - 192.168.0.1
         IPv6 :  01 fe 80 00 00 00 00 00 00 02 21 2f ff fe b5 6e 10 - fe80::221:2fff:feb5:6e10
         MAC  :  01 00 21 2f b5 6e 10 00 00 00 00 00 00 00 00 00 00 - 00:21:2f:b5:6e:10

     Let's suppose that we wanted to find a data node with just the IPv6 address.  We first memset()
     key[][] to 0.  We then add the IPv6 address with it's key boolean set to 1 and pass the key[][] array
     to the member functions.  Thus:

                 kb key bytes
         IPv4 :  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 - 0
         IPv6 :  01 fe 80 00 00 00 00 00 00 02 21 2f ff fe b5 6e 10 - fe80::221:2fff:feb5:6e10
         MAC  :  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 - 0

     See the member function summary above or a detailed member function usage comment before each
     function's code.  A verify() member function is provided that does extensive data structure memory
     analysis and a print() member function is provided that prints the structural details of all the
     branch and data nodes.

     For a trie of NUM_KEYS keys, each data node will have NUM_KEYS branch nodes associated with it.  The
     number of actual data nodes in the data structure is MAX_NUM_RDX_NODES+1.  This extra node is for the
     initial branch nodes and initial data node with the impossible key of all 0xff's(1,2,3).  The number
     of user storable nodes is MAX_NUM_RDX_NODES.   The user, through the constructor, declares PNODEs,
     each of which contains a radix PATRICIA trie of MAX_NUM_RDX_NODES nodes with NUM_KEYS keys of
     NUM_KEY_BYTES bytes length.

     These routines are a modification of the algorithm cited.  Specifically, the upward pointers used to
     terminate search are not used, and are instead used to point to data nodes as trie leaves.  In
     multi-key search with PATRICIA the upward pointer scheme will not work since each key would have
     pointed to a different branch node with it's associated data structure.  In multi-key PATRICIA search
     all keys must lead to the same data node.  The viability of multi-key radix PATRICIA search depends
     on the fact that each data node insertion requires only one new branch node for each key, thus the
     initial allocation of NUM_KEYS branch nodes with each data node.  This implementation has several
     additional branch and data node members not used in the original PATRICIA search algorithm.  These
     are for various purposes; related to changes in the original algorithm for multi-key search, related
     to changes in the original algorithm for leaf data nodes instead of upward pointers to data structures
     stored in branch nodes, related to printing, sorting and debugging.

======================================================================================================================

```

