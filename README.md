```
rdx
===

Multi-key Radix PATRICIA Fast Search C/OO C++
   - C and OO C++ versions
   - in which "PATRICIA--Practical Algorithm To Retrieve Information Coded in Alphanumeric," by Morrison, Donald R.
     ACM, Vol. 15, No. 4, Oct. 1968. is used to store data in a data structure(APP_DATA.h) of NUM_KEYS keys of
     arbitrary NUM_KEY_BYTES length and arbitrary MAX_NUM_RDX_NODES number of data nodes and access(search/delete)
     these nodes with an arbitrary subset of the keys
   - each data node requires exactly NUM_KEYS branch nodes, thus the entire data structure may be pre-allocated
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

add C++ and C Description sections

```

