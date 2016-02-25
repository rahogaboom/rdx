```

rdx
===

 Multi-key Radix PATRICIA Fast Search C/OO C++

     - C and OO C++ versions

     - allows the allocation of a fixed sized contiguous data store that holds data nodes of an arbitrary structure
       that may be accessed with any number of keys of any size with the PATRICIA (Practical Algorithm To Retrieve
       Information Coded In Alphanumeric)(1,2,3) fast search algorithm

     - see article in Embedded Systems Programming, Mar. 1997, "Highly dynamic Radix fast search algorithm with easy
       sort/deletion"(included)

     - see article in Embedded Systems Design, Jun. 2007, "Using multi-key radix PATRICIA fast search"(included)

     - many verification tests that show complex code usage examples

     - DESIGN GOALS:
           1. Flexibility
           2. Originality
           3. Verifiability
           4. Testing
           5. Performance

 OO C++ version:

   Description:
       The MKRdxPat class allows the allocation of a fixed sized contiguous data store that holds data nodes of
       an arbitrary structure that may be accessed with any number of keys of any size with the PATRICIA (Practical
       Algorithm To Retrieve Information Coded In Alphanumeric)(1,2,3) fast search algorithm.  MKRdxPat is particularly
       suited to applications the require complex data structures be contiguously stored and accessed with an algorithm
       of known fast character with any of several possible keys, either singly of several simultaneously.
  
       For example, a data structure that required either of an IPv4, IPv6 or MAC address key(s) to access data nodes.
       In this case NUM_KEYS would be 3 and NUM_KEY_BYTES would be 16(the longest - see below).
  
       The MKRdxPat class supports a data structure of MAX_NUM_RDX_NODES data nodes and NUM_KEYS keys per data node
       with keys of NUM_KEY_BYTES bytes.  The class constructor:
  
           MKRdxPat<app_data> *rdx = new MKRdxPat<app_data>(MAX_NUM_RDX_NODES, NUM_KEYS, NUM_KEY_BYTES);
  
       along with a:
  
           struct app_data
           {
               // user data
           };
  
       class constructor typename specifies the data structure.  A data node may be accessed with a single key or any
       number of keys simultaneously.  Keys must be unique within their key index(0 - NUM_KEYS-1), but not over
       different key indexes.  Member functions that require keys are passed the array:
  
           unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
  
       One extra prefix byte, a key boolean - required to be 0(ignore) or 1(search), is set with each key to specify
       if the key is to be used in the data node search.  It is suggested that the array be first memset() to 0:
  
           memset(key, 0, NUM_KEYS * (1+NUM_KEY_BYTES));
  
       and then only keys to be used copied in with the key boolean(set to 1) prepended.
  
       In most cases the necessary keys will be of different lengths.  The NUM_KEY_BYTES constructor argument would be
       set to the longest of these.  All other shorter keys would be left justified in the key[][] array, that is,
       start just after the key boolean.  Thus, for the example above, the IPv6 key would use the full 16 bytes, the
       IPv4 key would use the first four bytes after the key boolean(with the rest(12) set to 0) and the MAC key would
       use the first 6 bytes after the key boolean(with the rest(10) set to 0).  This was done to make the specification
       of the key[][] array and the code associated with processing it simpler.  Example:
  
                   kb key bytes
           IPv4 :  01 aa bb cc dd 00 00 00 00 00 00 00 00 00 00 00 00
           IPv6 :  01 aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp
           MAC  :  01 aa bb cc dd ee ff 00 00 00 00 00 00 00 00 00 00
  
       See the member function summary above or a detailed member function usage comment before each function's code.
       A verify() member function is provided that does extensive data structure memory analysis and a print() member
       function is provided that prints the structural details of all the branch and data nodes.
  
       For a trie of NUM_KEYS keys, each data node will have NUM_KEYS branch nodes associated with it.  The number of
       actual data nodes in the data structure is MAX_NUM_RDX_NODES+1.  This extra node is for the initial branch nodes
       and initial data node with the impossible key of all 0xff's(1,2,3).  The number of user storable nodes is
       MAX_NUM_RDX_NODES.   The user, through the constructor, declares PNODEs, each of which contains a radix PATRICIA
       trie of MAX_NUM_RDX_NODES nodes with NUM_KEYS keys of NUM_KEY_BYTES bytes length.
  
       These routines are a modification of the algorithm cited.  Specifically, the upward pointers used to terminate
       search are not used, and are instead used to point to data nodes as trie leaves.  In multi-key search with
       PATRICIA the upward pointer scheme will not work since each key would have pointed to a different branch node
       with it's associated data structure.  In multi-key PATRICIA search all keys must lead to the same data node.
       The viability of multi-key radix PATRICIA search depends on the fact that each data node insertion requires
       only one new branch node for each key, thus the initial allocation of NUM_KEYS branch nodes with each data node.
       This implementation has several additional branch and data node members not used in the original PATRICIA search
       algorithm.  These are for various purposes; related to changes in the original algorithm for multi-key search,
       related to changes in the original algorithm for leaf data nodes instead of upward pointers to data structures
       stored in branch nodes, related to printing, sorting and debugging.

```

