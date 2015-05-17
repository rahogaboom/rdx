
 rdx-1.0.6.tar.gz - 

    a. entirely a measuring performance release.  a new executable, rdx_pat_perf, is provided.  see
       the README.md file section under DESIGN GOALS on Performance.


 rdx-1.0.5.tar.gz - 

    a. new HISTORY.md file
    b. changed *test?.c file to not do some tests for one node/one key/one key byte case since
        these will always fail because these tests require three keys


 rdx-1.0.4.tar.gz - 

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


 rdx-1.0.3.tar.gz - 

     a. modified insert to use the same type keys array with key boolean bytes required to be set to 1
        (use all keys).  this makes the the keys array arguments uniform over all routines.
     b. updated and simplified the test code the reflect the change to the insert routine.
     c. ran tests with much larger MAX_NUM_RDX_NODES, NUM_KEYS and NUM_KEY_BYTES.


 rdx-1.0.2.tar.gz - 

     a. search/delete/print now only require a subset of keys(from 1 to NUM_KEYS keys) - insert still requires
        a full set of keys of course.
     b. the distinction between C and C++ code is removed.  only one version is provided.  the rdx_pat.mk
        script can be run with clang, gcc or g++.
     c. the test suite has been improved substantially.  new tests for search/delete/print with a single key
        have been added.  the rdx_pat_test.results file of test output should be much more readable and the
        correspondence of results with example code should be clearer.


 Why are there no releases beyond the latest three on GitHub?  For now, I only intend to maintain and answer
 questions about the most recent releases.  This may change in future.

