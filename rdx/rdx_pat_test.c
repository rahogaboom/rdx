/* test cases and example code for rdx_pat_XXX() library routines */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rdx_pat_search.h"

/*
 * Notes:
 *    1. using rand() may produce duplicate keys, especially for short keys.
 *    2. all tests use MAX_NUM_RDX_NODES node tries, NUM_KEYS keys and NUM_KEY_BYTES key length.
 *       these defines are set in rdx_pat_search.h.  modify these and recompile for different sized tries.
 *    3. if you fail to call rdx_pat_initialize() then absolutely anything can happen.
 *    4. it is suggested that the 'unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]' arguments to the
 *       insert/search/delete/print routines be first memset() to 0 and then the keys with the
 *       key boolean prepended copied.  a key will only be added if it's needed and the key boolean
 *       will be 1.  all other keys and their key booleans will be set to 0.
 */


/* for qsort() on a double field */
   static int
double_compare(
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


/* print all NUM_KEYS keys in a data node */
   void
print_keys(
             FILE *fp,
             DNODE *dnp
          )
{
   static unsigned int k, b;

   if ( dnp == NULL )
   {
      fprintf(fp, "Can't print data node keys: NULL data node\n");
   }
   else
   {
      for ( k = 0 ; k < NUM_KEYS ; k++ )
      {
         fprintf(fp, "   key %d = ", k);
         for ( b = 0 ; b < NUM_KEY_BYTES ; b++ )
         {
            fprintf(fp, "%02x ", dnp->key[k][b+1]);
         }
         fprintf(fp, "\n");
      }
      fprintf(fp, "\n");
   }
}


/* test cases and example code for rdx_pat_XXX() library routines */
   int
main()
{
   static unsigned int n, k, b; /* node number, key number, byte number */
   static unsigned int tot_nodes;
   static unsigned int tot_errs;
   static unsigned int return_code;
   static unsigned int test_num;


   /*
    * PNODE rdx1 related
    */
   static PNODE rdx1;

   /* holds sets of random keys for MAX_NUM_RDX_NODES sets with NUM_KEYS keys of NUM_KEY_BYTES length */
   static unsigned char rdx1_key[MAX_NUM_RDX_NODES][NUM_KEYS][1+NUM_KEY_BYTES];


   /*
    * PNODE rdx2 related
    */
   static PNODE rdx2;

   /* like rdx1_key[][][] except one extra set of keys allocated.  used in TEST 13 only. */
   static unsigned char rdx2_key[MAX_NUM_RDX_NODES+1][NUM_KEYS][1+NUM_KEY_BYTES];


   static DNODE *dnp; /* data node pointer */
   static DNODE **sorted_nodes; /* used to test rdx_pat_sort() in TEST 5 only */
   static FILE *fp; /* test output */


   fp = fopen("rdx_pat_test.results", "w");

   /* initialize rdx1 PNODE structure */
   rdx_pat_initialize(&rdx1);

   /*
    * in rdx1_key[][][] generate MAX_NUM_RDX_NODES sets of NUM_KEYS random keys each of
    * NUM_KEY_BYTES in length and set all key booleans to 1
    */
   for ( n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
   {
      for ( k = 0 ; k < NUM_KEYS ; k++ )
      {
         rdx1_key[n][k][0] = 1; /* set key boolean to 1 */
         for ( b = 1 ; b < 1+NUM_KEY_BYTES ; b++ )
         {
            rdx1_key[n][k][b] = rand() & 0x000000ff;
         }
      }
   }

   fprintf(fp, "####################################################################################################\n");
   fprintf(fp, "TEST 0: Print all allocated nodes plus the impossible key root node in rdx1 trie.\n");
   fprintf(fp, "        Expected Results:\n");
   fprintf(fp, "           a. Only the root node should be printed since no other nodes have been inserted\n\n");

   fprintf(fp, "TEST 0: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

   rdx_pat_print(&rdx1, NULL, fp);

   fprintf(fp, "TEST 0: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
   fprintf(fp, "\n\n\n");


   fprintf(fp, "####################################################################################################\n");
   fprintf(fp, "TEST 1: Insert one data node with NUM_KEYS random keys in rdx1 trie.\n");
   fprintf(fp, "        Expected Results:\n");
   fprintf(fp, "           a. One key insertion with return code 0\n");
   fprintf(fp, "           b. Total nodes allocated(not including root node) 1\n");
   fprintf(fp, "           c. No verification error\n");
   fprintf(fp, "           d. Print allocated node and root node\n\n");

   fprintf(fp, "TEST 1: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

   n = 0;

   return_code = rdx_pat_insert(&rdx1, rdx1_key[n], &dnp);

   fprintf(fp, "TEST 1: Node = %d Return Code = %d\n\n", n, return_code);

   fprintf(fp, "TEST 1: the *dnp data node keys:\n\n");
   print_keys(fp, dnp);

   /*
    * set some data in the APP_DATA struct of the data node - see rdx_pat_data.h.
    * used 'aabbccdd' to be readily recognizable in the test output.
    */
   dnp->data.id = 0xaabbccdd;

   fprintf(fp, "TEST 1: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

   fprintf(fp, "TEST 1: call rdx_pat_verify() with ERR_CODE_PRINT - print diagnostics:\n\n");
   rdx_pat_verify(&rdx1, ERR_CODE_PRINT, fp);

   return_code = rdx_pat_verify(&rdx1, ERR_CODE, fp);
   if ( return_code != 0 )
   {
      fprintf(fp, "TEST 1: rdx_pat_verify() verification error - %d\n\n", return_code);
   }
   else
   {
      fprintf(fp, "TEST 1: rdx_pat_verify() verification successful\n\n");
   }

   fprintf(fp, "TEST 1: call rdx_pat_verify() with NULL - print entire rdx1 trie:\n\n");
   rdx_pat_print(&rdx1, NULL, fp);

   fprintf(fp, "TEST 1: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
   fprintf(fp, "\n\n\n");


   fprintf(fp, "####################################################################################################\n");
   fprintf(fp, "TEST 2: Test rdx_pat_print() with key argument inserted in TEST 1 in rdx1 trie.\n");
   fprintf(fp, "        Expected Results:\n");
   fprintf(fp, "           a. Should print only the data node with the passed in keys and all of the branch\n");
   fprintf(fp, "              nodes leading to that data node.  If there are N keys in that data node then\n");
   fprintf(fp, "              there will be N branch node sequences that lead to the same data node\n\n");

   fprintf(fp, "TEST 2: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

   rdx_pat_print(&rdx1, rdx1_key[0], fp);

   fprintf(fp, "TEST 2: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
   fprintf(fp, "\n\n\n");


   if ( MAX_NUM_RDX_NODES >= 5 )
   {
      fprintf(fp, "####################################################################################################\n");
      fprintf(fp, "TEST 3: Insert four data nodes each with NUM_KEYS random keys in rdx1 trie.\n");
      fprintf(fp, "        Expected Results:\n");
      fprintf(fp, "           a. Four data node insertions with return code 0\n");
      fprintf(fp, "           b. Total nodes allocated(not including root node) 5(4 here and 1 from TEST 1)\n");
      fprintf(fp, "           c. No verification error\n\n");

      fprintf(fp, "TEST 3: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

      for ( n = 1 ; n < 5 ; n++ )
      {
         return_code = rdx_pat_insert(&rdx1, rdx1_key[n], &dnp);

         fprintf(fp, "TEST 3: Node = %d Return Code = %d\n\n", n, return_code);

         fprintf(fp, "TEST 3: the *dnp data node keys:\n\n");
         print_keys(fp, dnp);

         if ( return_code == 0 )
         {
            /*
             * set some data in the APP_DATA struct of the data node - see rdx_pat_data.h.
             * used 'aabbccdd' to be readily recognizable in the test output.
             */
            dnp->data.id = 0xaabbccdd;
         }
      }
      fprintf(fp, "TEST 3: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

      fprintf(fp, "TEST 3: call rdx_pat_verify() with ERR_CODE_PRINT - print diagnostics:\n\n");
      return_code = rdx_pat_verify(&rdx1, ERR_CODE_PRINT, fp);
      if ( return_code != 0 )
      {
         fprintf(fp, "TEST 3: rdx_pat_verify() verification error - %d\n", return_code);
      }
      else
      {
         fprintf(fp, "TEST 3: rdx_pat_verify() verification successful\n");
      }
   }
   else
   {
      fprintf(fp, "TEST 3: Not done - needs MAX_NUM_RDX_NODES >= 5.\n");
   }

   fprintf(fp, "TEST 3: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
   fprintf(fp, "\n\n\n");


   fprintf(fp, "####################################################################################################\n");
   fprintf(fp, "TEST 4: Search for all MAX_NUM_RDX_NODES data nodes with NUM_KEYS random keys in rdx1 trie.\n");
   fprintf(fp, "        Expected Results:\n");
   fprintf(fp, "           a. find all 5 data nodes previously inserted\n");
   fprintf(fp, "           b. fail to find 3 nodes who's keys were not inserted\n\n");

   fprintf(fp, "TEST 4: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

   for ( n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
   {
      dnp = rdx_pat_search(&rdx1, rdx1_key[n]);
      if (dnp == NULL)
      {
         fprintf(fp, "TEST 4: Node = %d NULL return - search fail\n", n);
      }
      else
      {
         fprintf(fp, "TEST 4: Node = %d\n\n", n);

         fprintf(fp, "TEST 4: the *dnp data node keys:\n\n");
         print_keys(fp, dnp);
      }
   }

   fprintf(fp, "TEST 4: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
   fprintf(fp, "\n\n\n");


   fprintf(fp, "####################################################################################################\n");
   fprintf(fp, "TEST 5: Sort data nodes by successive keys in rdx1 trie\n");
   fprintf(fp, "        Expected Results:\n");
   fprintf(fp, "           a. For each key the return code will equal the number of sorted nodes and the\n");
   fprintf(fp, "              nodes array will hold the array of node pointers to nodes in sorted order\n\n");

   fprintf(fp, "TEST 5: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

   for ( k = 0 ; k < NUM_KEYS ; k++ )
   {
      fprintf(fp, "TEST 5: Sort the nodes by key %d.\n\n", k);
      return_code = rdx_pat_sort(&rdx1, &sorted_nodes, k);
      for ( n = 0 ; n < return_code ; n++ )
      {
         if ( sorted_nodes[n] == NULL )
         {
            fprintf(fp, "TEST 5: n = %d NULL\n", n);
         }
         else
         {
            fprintf(fp, "TEST 5: n = %d  data.id = %08x\n\n", n, ((DNODE *)sorted_nodes[n])->data.id);

            fprintf(fp, "TEST 5: the *dnp data node keys:\n\n");
            print_keys(fp, (DNODE *)sorted_nodes[n]);
         }
      }
      fprintf(fp, "TEST 5: rdx1 - Total sorted keys = %d\n\n", return_code);
   }

   fprintf(fp, "TEST 5: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
   fprintf(fp, "\n\n\n");


   fprintf(fp, "####################################################################################################\n");
   fprintf(fp, "TEST 6: Test to show how to sort nodes based on an arbitrary sort key in the APP_DATA struct\n");
   fprintf(fp, "        Expected Results:\n");
   fprintf(fp, "           a. Using data.data1.d double to sort on.  Nodes should be ordered by this field\n\n");

   fprintf(fp, "TEST 6: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

   tot_nodes = rdx_pat_nodes(&rdx1);
   fprintf(fp, "TEST 6: tot_nodes = %d\n", tot_nodes);

   for ( n = 0 ; n < tot_nodes ; n++ )
   {
      ((DNODE *)rdx1.node_ptrs[n])->data.data1.d = n;
      fprintf(fp, "TEST 6: node %d  0x%lx %f\n",
         n, (unsigned long)(DNODE *)rdx1.node_ptrs[n], ((DNODE *)rdx1.node_ptrs[n])->data.data1.d);
   }
   fprintf(fp, "\n");

   qsort(&rdx1.node_ptrs[0], tot_nodes, sizeof(DNODE *), double_compare);

   for ( n = 0 ; n < tot_nodes ; n++ )
   {
      fprintf(fp, "TEST 6: node %d  0x%lx %f\n",
         n, (unsigned long)(DNODE *)rdx1.node_ptrs[n], ((DNODE *)rdx1.node_ptrs[n])->data.data1.d);
   }
   fprintf(fp, "\n");

   fprintf(fp, "TEST 6: call rdx_pat_verify() with ERR_CODE_PRINT - print diagnostics:\n\n");
   return_code = rdx_pat_verify(&rdx1, ERR_CODE_PRINT, fp);
   if ( return_code != 0 )
   {
      fprintf(fp, "TEST 6: rdx_pat_verify() verification error - %d\n", return_code);
   }
   else
   {
      fprintf(fp, "TEST 6: rdx_pat_verify() verification successful\n");
   }

   fprintf(fp, "TEST 6: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
   fprintf(fp, "\n\n\n");


   fprintf(fp, "####################################################################################################\n");
   fprintf(fp, "TEST 7: Delete all keys in rdx1 trie\n");
   fprintf(fp, "        Expected Results:\n");
   fprintf(fp, "           a. Non-NULL returns of the nodes deleted, NULL returns for nodes not\n");
   fprintf(fp, "              allocated and zero allocated nodes upon completion\n\n");

   fprintf(fp, "TEST 7: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

   for ( n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
   {
      dnp = rdx_pat_delete(&rdx1, rdx1_key[n]);
      if (dnp == NULL)
      {
         fprintf(fp, "TEST 7: Node = %d NULL return - delete fail\n", n);
      }
      else
      {
         fprintf(fp, "TEST 7: Node = %d\n\n", n);

         fprintf(fp, "TEST 7: the *dnp data node keys:\n\n");
         print_keys(fp, dnp);
      }
   }
   fprintf(fp, "\n");

   fprintf(fp, "TEST 7: call rdx_pat_verify() with ERR_CODE_PRINT - print diagnostics:\n\n");
   return_code = rdx_pat_verify(&rdx1, ERR_CODE_PRINT, fp);
   if ( return_code != 0 )
   {
      fprintf(fp, "TEST 7: rdx_pat_verify() verification error - %d\n", return_code);
   }
   else
   {
      fprintf(fp, "TEST 7: rdx_pat_verify() verification successful\n");
   }

   fprintf(fp, "TEST 7: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
   fprintf(fp, "\n\n\n");


   fprintf(fp, "####################################################################################################\n");
   fprintf(fp, "TEST 8: Insert/Search/Delete MAX_NUM_RDX_NODES nodes with random keys repeatedly.\n");
   fprintf(fp, "        Expected Results:\n");
   fprintf(fp, "           a. Do not report success - report only errors; insert/search/delete operation failures\n\n");

   fprintf(fp, "TEST 8: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

   for ( test_num = 0,tot_errs = 0 ; test_num < 32 ; test_num++ )
   {
      unsigned int n;

      /* in rdx1_key[][][] generate MAX_NUM_RDX_NODES sets of NUM_KEYS random keys each of NUM_KEY_BYTES in length */
      for ( n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
      {
         for ( k = 0 ; k < NUM_KEYS ; k++ )
         {
            rdx1_key[n][k][0] = 1; /* set key boolean to 1 */
            for ( b = 1 ; b < 1+NUM_KEY_BYTES ; b++ )
            {
               rdx1_key[n][k][b] = rand() & 0x000000ff;
            }
         }
      }

      /* insert full set of MAX_NUM_RDX_NODES data nodes */
      for ( n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
      {
         return_code = rdx_pat_insert(&rdx1, rdx1_key[n], &dnp);
         if ( return_code != 0 )
         {
            fprintf(fp, "TEST 8: rdx_pat_insert() error - %d\n", return_code);
            tot_errs++;
         }
      }

      /* search for full set of MAX_NUM_RDX_NODES data nodes */
      for ( n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
      {
         dnp = rdx_pat_search(&rdx1, rdx1_key[n]);
         if (dnp == NULL)
         {
            fprintf(fp, "TEST 8: rdx_pat_search() error\n");
            tot_errs++;
         }
      }

      /* delete a full set of MAX_NUM_RDX_NODES data nodes */
      for ( n = 0 ; n < MAX_NUM_RDX_NODES ; n++ )
      {
         dnp = rdx_pat_delete(&rdx1, rdx1_key[n]);
         if (dnp == NULL)
         {
            fprintf(fp, "TEST 8: rdx_pat_delete() error\n");
            tot_errs++;
         }
      }

      fprintf(fp, "TEST 8: test = %d  tot_errs = %d\n", test_num, tot_errs);
      fflush(fp);
   }
   fprintf(fp, "TEST 8 - Total Insert/Search/Delete tests run %d  Total errors detected %d\n", test_num, tot_errs);

   fprintf(fp, "TEST 8: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
   fprintf(fp, "\n\n\n");


   if ( NUM_KEYS == 3 && MAX_NUM_RDX_NODES >= 2 )
   {
      fprintf(fp, "####################################################################################################\n");
      fprintf(fp, "TEST 9: Insert two data nodes in rdx1 with specific keys(keys 1,2,3 in data node 0 and keys 4,5,6 in data node 1).\n");
      fprintf(fp, "        Verify inserts worked - verify searches/deletes with mixed up keys fail.\n");
      fprintf(fp, "        Expected Results:\n");
      fprintf(fp, "           a. search on the two inserted data nodes should not report errors\n");
      fprintf(fp, "           b. search on mixed up keys from both data nodes should fail\n");
      fprintf(fp, "           c. delete on mixed up keys from both data nodes should fail\n\n");

      fprintf(fp, "TEST 9: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

      for ( k = 0 ; k < NUM_KEYS ; k++ )
      {
         for ( b = 1 ; b < 1+NUM_KEY_BYTES ; b++ )
         {
            rdx1_key[0][k][b] = 0;
            rdx1_key[1][k][b] = 0;
         }
      }
      rdx1_key[0][0][NUM_KEY_BYTES] = 1;
      rdx1_key[0][1][NUM_KEY_BYTES] = 2;
      rdx1_key[0][2][NUM_KEY_BYTES] = 3;

      rdx1_key[1][0][NUM_KEY_BYTES] = 4;
      rdx1_key[1][1][NUM_KEY_BYTES] = 5;
      rdx1_key[1][2][NUM_KEY_BYTES] = 6;

      return_code = rdx_pat_insert(&rdx1, rdx1_key[0], &dnp);
      fprintf(fp, "TEST 9: Node = %d Return Code = %d\n\n", 0, return_code);

      fprintf(fp, "TEST 9: the *dnp data node keys:\n\n");
      print_keys(fp, dnp);

      return_code = rdx_pat_insert(&rdx1, rdx1_key[1], &dnp);
      fprintf(fp, "TEST 9: Node = %d Return Code = %d\n\n", 1, return_code);

      fprintf(fp, "TEST 9: the *dnp data node keys:\n\n");
      print_keys(fp, dnp);

      dnp = rdx_pat_search(&rdx1, rdx1_key[0]);
      if (dnp == NULL)
      {
         fprintf(fp, "TEST 9: rdx_pat_search() - first search fail - unexpected.\n");
      }
      else
      {
         fprintf(fp, "TEST 9: rdx_pat_search() - first search succeeds - expected.\n");
      }

      dnp = rdx_pat_search(&rdx1, rdx1_key[1]);
      if (dnp == NULL)
      {
         fprintf(fp, "TEST 9: rdx_pat_search() - second search fail - unexpected.\n");
      }
      else
      {
         fprintf(fp, "TEST 9: rdx_pat_search() - second search succeeds - expected.\n");
      }
      fprintf(fp, "\n");

      rdx1_key[0][0][NUM_KEY_BYTES] = 1;
      rdx1_key[0][1][NUM_KEY_BYTES] = 2;
      rdx1_key[0][2][NUM_KEY_BYTES] = 6;

      fprintf(fp, "TEST 9: rdx_pat_search() - search for data node with keys 1,2,6.\n");
      dnp = rdx_pat_search(&rdx1, rdx1_key[0]);
      if (dnp == NULL)
      {
         fprintf(fp, "TEST 9: rdx_pat_search() - search fails - keys in different data nodes - expected.\n");
      }
      fprintf(fp, "\n");

      fprintf(fp, "TEST 9: rdx_pat_delete() - delete data node with keys 1,2,6.\n");
      dnp = rdx_pat_delete(&rdx1, rdx1_key[0]);
      if (dnp == NULL)
      {
         fprintf(fp, "TEST 9: rdx_pat_delete() - delete fails - keys in different data nodes - expected.\n");
      }
      fprintf(fp, "\n");

      rdx1_key[1][0][NUM_KEY_BYTES] = 4;
      rdx1_key[1][1][NUM_KEY_BYTES] = 5;
      rdx1_key[1][2][NUM_KEY_BYTES] = 3;

      fprintf(fp, "TEST 9: rdx_pat_search() - search for data node with keys 4,5,3.\n");
      dnp = rdx_pat_search(&rdx1, rdx1_key[1]);
      if (dnp == NULL)
      {
         fprintf(fp, "TEST 9: rdx_pat_search() - search fails - keys in different data nodes - expected.\n");
      }
      fprintf(fp, "\n");

      fprintf(fp, "TEST 9: rdx_pat_delete() - delete data node with keys 4,5,3.\n");
      dnp = rdx_pat_delete(&rdx1, rdx1_key[1]);
      if (dnp == NULL)
      {
         fprintf(fp, "TEST 9: rdx_pat_delete() - delete fails - keys in different data nodes - expected.\n");
      }

      fprintf(fp, "TEST 9: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
   }
   else
   {
      fprintf(fp, "####################################################################################################\n");
      fprintf(fp, "TEST 9: Not done - needs NUM_KEYS = 3 and MAX_NUM_RDX_NODES >= 2.\n");
   }
   fprintf(fp, "\n\n\n");


   if ( NUM_KEYS == 3 && MAX_NUM_RDX_NODES >= 2 )
   {
      fprintf(fp, "####################################################################################################\n");
      fprintf(fp, "TEST 10: Search for one data node in rdx1 inserted in TEST 9(keys 1,2,3) with one key.\n");
      fprintf(fp, "         Expected Results:\n");
      fprintf(fp, "            a. node is found with only key 0(value 1) and keys 1(value 2) and 2(value 3) are ignored.\n");
      fprintf(fp, "               number of nodes remains at two since a search does not change the number of nodes.\n\n");

      fprintf(fp, "TEST 10: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

      /* a data node with these keys was inserted in TEST 9 */
      rdx1_key[0][0][NUM_KEY_BYTES] = 1;
      rdx1_key[0][1][NUM_KEY_BYTES] = 2;
      rdx1_key[0][2][NUM_KEY_BYTES] = 3;

      rdx1_key[0][0][0] = 1; /* use first key(0) in search */
      rdx1_key[0][1][0] = 0; /* ignore second key(1) in search */
      rdx1_key[0][2][0] = 0; /* ignore third key(2) in search */
      dnp = rdx_pat_search(&rdx1, rdx1_key[0]);
      if (dnp == NULL)
      {
         fprintf(fp, "TEST 10: NULL return - search failed using only first(key 0 value 1) of three keys\n");
      }
      else
      {
         fprintf(fp, "TEST 10: search using only first(key 0 value 1) of three keys succeeded\n\n");

         fprintf(fp, "TEST 10: the *dnp data node keys:\n\n");
         print_keys(fp, dnp);
      }

      fprintf(fp, "TEST 10: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
   }
   else
   {
      fprintf(fp, "####################################################################################################\n");
      fprintf(fp, "TEST 10: Not done - needs NUM_KEYS = 3 and MAX_NUM_RDX_NODES >= 2.\n");
   }
   fprintf(fp, "\n\n\n");


   if ( NUM_KEYS == 3 && MAX_NUM_RDX_NODES >= 2 )
   {
      fprintf(fp, "####################################################################################################\n");
      fprintf(fp, "TEST 11: Delete one data node in rdx1 inserted in TEST 9(keys 4,5,6) with one key.\n");
      fprintf(fp, "         Expected Results:\n");
      fprintf(fp, "            a. node is deleted with only key 2(value 6) and keys 1(value 5) and 0(value 4) are ignored.\n");
      fprintf(fp, "               number of nodes decreases by one because of the delete.\n\n");

      fprintf(fp, "TEST 11: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

      /* a data node with these keys was inserted in TEST 9 */
      rdx1_key[1][0][NUM_KEY_BYTES] = 4;
      rdx1_key[1][1][NUM_KEY_BYTES] = 5;
      rdx1_key[1][2][NUM_KEY_BYTES] = 6;

      rdx1_key[1][0][0] = 0; /* ignore first key(0) in search */
      rdx1_key[1][1][0] = 0; /* ignore second key(1) in search */
      rdx1_key[1][2][0] = 1; /* use third key(2) in search */
      dnp = rdx_pat_delete(&rdx1, rdx1_key[1]);
      if (dnp == NULL)
      {
         fprintf(fp, "TEST 11: NULL return - delete failed using only third(key 2 value 6) of three keys\n");
      }
      else
      {
         fprintf(fp, "TEST 11: delete using only third(key 2 value 6) of three keys succeeded\n\n");

         fprintf(fp, "TEST 11: the *dnp data node keys:\n\n");
         print_keys(fp, dnp);
      }

      fprintf(fp, "TEST 11: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
   }
   else
   {
      fprintf(fp, "####################################################################################################\n");
      fprintf(fp, "TEST 11: Not done - needs NUM_KEYS = 3 and MAX_NUM_RDX_NODES >= 2.\n");
   }
   fprintf(fp, "\n\n\n");


   if ( NUM_KEYS == 3 && MAX_NUM_RDX_NODES >= 2 )
   {
      fprintf(fp, "####################################################################################################\n");
      fprintf(fp, "TEST 12: Print one data node inserted in TEST 9(keys 1,2,3) with one key.\n");
      fprintf(fp, "         Expected Results:\n");
      fprintf(fp, "            a. node is printed with only key 1(value 2) and keys 0(value 1) and 2(value 3) are ignored\n\n");

      fprintf(fp, "TEST 12: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));

      /* a data node with these keys was inserted in TEST 9 */
      rdx1_key[0][0][NUM_KEY_BYTES] = 1;
      rdx1_key[0][1][NUM_KEY_BYTES] = 2;
      rdx1_key[0][2][NUM_KEY_BYTES] = 3;

      rdx1_key[0][0][0] = 0; /* ignore first key(0) in search */
      rdx1_key[0][1][0] = 1; /* use second key(1) in search */
      rdx1_key[0][2][0] = 0; /* ignore third key(2) in search */
      rdx_pat_print(&rdx1, rdx1_key[0], fp);

      fprintf(fp, "TEST 12: rdx1 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx1));
   }
   else
   {
      fprintf(fp, "####################################################################################################\n");
      fprintf(fp, "TEST 12: Not done - needs NUM_KEYS = 3 and MAX_NUM_RDX_NODES >= 2.\n");
   }
   fprintf(fp, "\n\n\n");


   /* initialize rdx2 PNODE structures */
   rdx_pat_initialize(&rdx2);

   /* in rdx2_key[][][] generate MAX_NUM_RDX_NODES+1 sets of NUM_KEYS random keys each of NUM_KEY_BYTES in length */
   for ( n = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
   {
      for ( k = 0 ; k < NUM_KEYS ; k++ )
      {
         rdx2_key[n][k][0] = 1; /* set key boolean to 1 */
         for ( b = 1 ; b < 1+NUM_KEY_BYTES ; b++ )
         {
            rdx2_key[n][k][b] = rand() & 0x000000ff;
         }
      }
   }


   fprintf(fp, "####################################################################################################\n");
   fprintf(fp, "TEST 13: Insert MAX_NUM_RDX_NODES+1 data nodes of NUM_KEYS random keys in rdx2 trie.\n");
   fprintf(fp, "         Expected Results:\n");
   fprintf(fp, "            a. MAX_NUM_RDX_NODES key insertions with return code 0\n");
   fprintf(fp, "            b. 1 key insertion with return code 2(no free nodes)\n");
   fprintf(fp, "            c. Total nodes allocated(not including root node) MAX_NUM_RDX_NODES\n");
   fprintf(fp, "            d. No verification error\n\n");

   fprintf(fp, "TEST 13: rdx2 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx2));

   for ( n = 0 ; n < MAX_NUM_RDX_NODES+1 ; n++ )
   {
      return_code = rdx_pat_insert(&rdx2, rdx2_key[n], &dnp);

      fprintf(fp, "TEST 13: Node = %d Return Code = %d\n\n", n, return_code);

      fprintf(fp, "TEST 13: the *dnp data node keys:\n\n");
      print_keys(fp, dnp);

      if ( dnp != NULL )
      {
         /*
          * set some data in the APP_DATA struct of the data node - see rdx_pat_data.h.
          * used 'aabbccdd' to be readily recognizable in the test output.
          */
         dnp->data.id = 0xaabbccdd;
      }
   }
   fprintf(fp, "TEST 13: rdx2 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx2));

   fprintf(fp, "TEST 13: call rdx_pat_verify() with ERR_CODE_PRINT - print diagnostics:\n\n");
   return_code = rdx_pat_verify(&rdx2, ERR_CODE_PRINT, fp);
   if ( return_code != 0 )
   {
      fprintf(fp, "TEST 13: rdx_pat_verify() verification error - %d\n", return_code);
   }
   else
   {
      fprintf(fp, "TEST 13: rdx_pat_verify() verification successful\n");
   }

   fprintf(fp, "TEST 13: rdx2 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx2));
   fprintf(fp, "\n\n\n");


   fprintf(fp, "####################################################################################################\n");
   fprintf(fp, "TEST 14: Test rdx_pat_print() with NULL key argument on rdx2 trie\n");
   fprintf(fp, "         Expected Results:\n");
   fprintf(fp, "            a. Should print all branch and data nodes in rdx2 trie\n\n");

   fprintf(fp, "TEST 14: rdx2 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx2));

   rdx_pat_print(&rdx2, NULL, fp);

   fprintf(fp, "TEST 14: rdx2 - Nodes allocated = %d\n\n", rdx_pat_nodes(&rdx2));

   return 0;
}

