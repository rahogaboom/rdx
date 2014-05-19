/* tests for rdx routines - examples of usage */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rdx_pat_search.h"

/*
 * Notes:
 *    1. using rand() may produce duplicate keys, especially for short keys.
 *    2. all tests are with MAX_NUM_RDX_NODES node tries, NUM_KEYS keys and NUM_KEY_BYTES key length.
 *       these defines are set in rdx_pat_search.h.  modify these and recompile for different sized tries.
 *    3. if you fail to call rdx_pat_initialize() then absolutely anything can happen.
 */


/* for qsort() on a double field */
   static int
double_compare(
   const void *a,
   const void *b)
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


/* generate NUM_KEYS random keys each of NUM_KEY_BYTES in length */
   void
gen_keys(
   unsigned char keys[NUM_KEYS][NUM_KEY_BYTES])
{
   static unsigned int k, b;

   for ( k=0 ; k<NUM_KEYS ; k++ )
   {
      for ( b=0 ; b<NUM_KEY_BYTES ; b++ )
      {
         keys[k][b] = rand() & 0x000000ff;
      }
   }
}


/* print all NUM_KEYS keys in a data node */
   void
print_keys(
   FILE *fp,
   DNODE *dnp)
{
   static unsigned int k, b;

   if ( dnp == NULL )
   {
      fprintf(fp, "Can't print data node keys: NULL data node\n");
   }
   else
   {
      for ( k=0 ; k<NUM_KEYS ; k++ )
      {
         fprintf(fp, "   key %d = ", k);
         for ( b=0 ; b<NUM_KEY_BYTES ; b++ )
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
   static unsigned int k, b;
   static unsigned int tot_nodes;
   static unsigned int tot_errs;
   static unsigned int node1;
   static unsigned int node2;
   static unsigned int return_code;

   static PNODE rdx_data1;
   static unsigned char rdx_data1_keys[MAX_NUM_RDX_NODES][NUM_KEYS][NUM_KEY_BYTES];

   static PNODE rdx_data2;
   static unsigned char rdx_data2_keys[MAX_NUM_RDX_NODES+1][NUM_KEYS][NUM_KEY_BYTES];

   static DNODE *dnp;
   static DNODE **nodes;
   static FILE *fp;


   memset(rdx_data1_keys, 0, sizeof(rdx_data1_keys));
   memset(rdx_data2_keys, 0, sizeof(rdx_data2_keys));

   fp = fopen("rdx_pat_test.results", "w");

   /* initialize rdx_data1 structure */
   rdx_pat_initialize(&rdx_data1);



   /*
    * TEST 0 - Print all allocated nodes plus the impossible key root node in rdx_data1 trie
    *          Expected Results:
    *             a. Only the root node should be printed since no other nodes have had insertions
    */
   fprintf(fp, "TEST 0 - Print all allocated nodes plus the impossible key root node in rdx_data1 trie\n");
   rdx_pat_print(&rdx_data1, NULL, fp);
   fprintf(fp, "\n\n\n");



   /*
    * TEST 1 - Insert one random key in rdx_data1 trie
    *          Expected Results:
    *             a. One key insertion with return code 0
    *             b. Total nodes allocated(not including root node) 1
    *             c. No verification error
    *             d. Print allocated node and root node
    */
   fprintf(fp, "TEST 1 - Insert one random key in rdx_data1 trie\n");
   for ( node1=0 ; node1<1 ; node1++ )
   {
      gen_keys(rdx_data1_keys[node1]);

      return_code = rdx_pat_insert(&rdx_data1, rdx_data1_keys[node1], &dnp);

      fprintf(fp, "Node = %4d Return Code = %d\n", node1, return_code);
      print_keys(fp, dnp);

      dnp->data.id = rand();
   }
   fprintf(fp, "rdx_data1 - Nodes allocated = %d\n", rdx_pat_nodes(&rdx_data1));

   rdx_pat_verify(&rdx_data1, ERR_CODE_PRINT, fp);
   return_code = rdx_pat_verify(&rdx_data1, ERR_CODE, fp);
   if ( return_code != 0 )
   {
      fprintf(fp, "rdx_pat_verify() verification error in TEST 1 - %d\n", return_code);
   }
   else
   {
      fprintf(fp, "rdx_pat_verify() verification successful in TEST 1 \n");
   }

   rdx_pat_print(&rdx_data1, NULL, fp);
   fprintf(fp, "\n\n\n");



   /*
    * TEST 2 - Test rdx_pat_print() with key argument inserted in TEST 1 in rdx_data1 trie
    *          Expected Results:
    *             a. Should print only the data node with the passed in keys and all of the
    *                branch nodes leading to that data node.  If there are N keys in that
    *                data node then there will be N branch node sequences that lead to the
    *                same data node
    */
   fprintf(fp, "TEST 2 - Test rdx_pat_print() with key argument inserted in TEST 1 in rdx_data1 trie\n");
   rdx_pat_print(&rdx_data1, rdx_data1_keys[0], fp);
   fprintf(fp, "\n\n\n");



   /*
    * TEST 3 - Insert four random keys in rdx_data1 trie
    *          Expected Results:
    *             a. Four key insertions with return code 0
    *             b. Total nodes allocated(not including root node) 5
    *             c. No verification error
    */
   if ( MAX_NUM_RDX_NODES >= 5 )
   {
      fprintf(fp, "TEST 3 - Insert four random keys in rdx_data1 trie\n");
      for ( ; node1<5 ; node1++ )
      {
         gen_keys(rdx_data1_keys[node1]);

         return_code = rdx_pat_insert(&rdx_data1, rdx_data1_keys[node1], &dnp);

         fprintf(fp, "Node = %4d Return Code = %d\n", node1, return_code);
         print_keys(fp, dnp);

         if ( return_code == 0 )
         {
            dnp->data.id = rand();
         }
      }
      fprintf(fp, "rdx_data1 - Nodes allocated = %d\n", rdx_pat_nodes(&rdx_data1));

      return_code = rdx_pat_verify(&rdx_data1, ERR_CODE_PRINT, fp);
      if ( return_code != 0 )
      {
         fprintf(fp, "rdx_pat_verify() verification error in TEST 3 - %d\n", return_code);
      }
      else
      {
         fprintf(fp, "rdx_pat_verify() verification successful in TEST 3\n");
      }
   }
   else
   {
      fprintf(fp, "TEST 3 - Not done - needs MAX_NUM_RDX_NODES >= 5.\n");
   }
   fprintf(fp, "\n\n\n");



   /* initialize rdx_data2 structures */
   rdx_pat_initialize(&rdx_data2);



   /*
    * TEST 4 - Insert MAX_NUM_RDX_NODES+1 random keys in rdx_data2 trie
    *          Expected Results:
    *             a. MAX_NUM_RDX_NODES key insertions with return code 0
    *             b. 1 key insertion with return code 2(no free nodes)
    *             c. Total nodes allocated(not including root node) MAX_NUM_RDX_NODES
    *             d. No verification error
    */
   fprintf(fp, "TEST 4 - Insert MAX_NUM_RDX_NODES+1 random keys in rdx_data2 trie\n");
   for ( node2=0 ; node2<MAX_NUM_RDX_NODES+1 ; node2++ )
   {
      gen_keys(rdx_data2_keys[node2]);

      return_code = rdx_pat_insert(&rdx_data2, rdx_data2_keys[node2], &dnp);

      fprintf(fp, "Node = %4d Return Code = %d\n", node2, return_code);
      print_keys(fp, dnp);

      if ( dnp != NULL )
      {
         dnp->data.id = rand();
      }
   }
   fprintf(fp, "rdx_data2 - Nodes allocated = %d\n", rdx_pat_nodes(&rdx_data2));

   return_code = rdx_pat_verify(&rdx_data2, ERR_CODE_PRINT, fp);
   if ( return_code != 0 )
   {
      fprintf(fp, "rdx_pat_verify() verification error in TEST 4 - %d\n", return_code);
   }
   else
   {
      fprintf(fp, "rdx_pat_verify() verification successful in TEST 4\n");
   }
   fprintf(fp, "\n\n\n");


   /*
    * TEST 5 - Test rdx_pat_print() with NULL key argument on rdx_data2 trie
    *          Expected Results:
    *             a. Should print all branch and data nodes in rdx_data2 trie
    */
   fprintf(fp, "TEST 5 - Test rdx_pat_print() with NULL key argument on rdx_data2 trie\n");
   rdx_pat_print(&rdx_data2, NULL, fp);
   fprintf(fp, "\n\n\n");


   /*
    * TEST 6 - Search for all MAX_NUM_RDX_NODES random keys in rdx_data1 trie
    *          Expected Results:
    *             a. find all 5 nodes previously inserted
    *             c. fail to find 3 nodes who's keys were not inserted
    */
   fprintf(fp, "TEST 6 - Search for all MAX_NUM_RDX_NODES random keys in rdx_data1 trie\n");
   for ( node1=0 ; node1<MAX_NUM_RDX_NODES ; node1++ )
   {
      dnp = rdx_pat_search(&rdx_data1, rdx_data1_keys[node1]);
      if (dnp == NULL)
      {
         fprintf(fp, "Node = %4d NULL return - search fail\n", node1);
      }
      else
      {
         fprintf(fp, "Node = %4d\n", node1);
         print_keys(fp, dnp);
      }
   }
   fprintf(fp, "\n\n\n");



   /*
    * TEST 7 - Sort data nodes by successive keys in rdx_data1 trie
    *          Expected Results:
    *             a. For each key the return code will equal the number of sorted nodes
    *                and the nodes array will hold the array of node pointers to nodes
    *                in sorted order
    */
   fprintf(fp, "TEST 7 - Sort data nodes by successive keys in rdx_data1 trie\n");
   for ( k=0 ; k<NUM_KEYS ; k++ )
   {
      fprintf(fp, "Sort the nodes by key %d.\n", k);
      return_code = rdx_pat_sort(&rdx_data1, &nodes, k);
      for ( node1=0 ; node1<return_code ; node1++ )
      {
         if ( nodes[node1] == NULL )
         {
            fprintf(fp, "%d NULL\n", node1);
         }
         else
         {
            fprintf(fp, "data.id = %08x\n", ((DNODE *)nodes[node1])->data.id);
            print_keys(fp, (DNODE *)nodes[node1]);
         }
      }
      fprintf(fp, "rdx_data1 - Total sorted keys = %d\n", return_code);
   }
   fprintf(fp, "\n\n\n");



   /*
    * TEST 8 - Test to show how to sort nodes based on an arbitrary sort key in the APP_DATA struct.
    *          Expected Results:
    *             a. Using data.data1.d double to sort on.  Nodes should be ordered by this field.
    */
   fprintf(fp, "TEST 8 - Test to show how to sort nodes based on an arbitrary sort key in the APP_DATA struct\n");
   tot_nodes = rdx_pat_nodes(&rdx_data1);
   fprintf(fp, "tot_nodes = %d\n", tot_nodes);
   for ( node1=0 ; node1<tot_nodes ; node1++ )
   {
      ((DNODE *)rdx_data1.node_ptrs[node1])->data.data1.d = node1;
      fprintf(fp, "node %d  0x%08lx %f\n",
         node1, (unsigned long)(DNODE *)rdx_data1.node_ptrs[node1], ((DNODE *)rdx_data1.node_ptrs[node1])->data.data1.d);
   }
   fprintf(fp, "\n");
   qsort(&rdx_data1.node_ptrs[0], tot_nodes, sizeof(DNODE *), double_compare);
   for ( node1=0 ; node1<tot_nodes ; node1++ )
   {
      fprintf(fp, "node %d  0x%08lx %f\n",
         node1, (unsigned long)(DNODE *)rdx_data1.node_ptrs[node1], ((DNODE *)rdx_data1.node_ptrs[node1])->data.data1.d);
   }
   fprintf(fp, "\n");

   return_code = rdx_pat_verify(&rdx_data1, ERR_CODE_PRINT, fp);
   if ( return_code != 0 )
   {
      fprintf(fp, "rdx_pat_verify() verification error in TEST 8 - %d\n", return_code);
   }
   else
   {
      fprintf(fp, "rdx_pat_verify() verification successful in TEST 8\n");
   }
   fprintf(fp, "\n\n\n");



   /*
    * TEST 9 - Delete all keys in rdx_data1 trie
    *          Expected Results:
    *             a. Non-NULL returns of the nodes deleted, NULL returns for nodes not
    *                allocated and zero allocated nodes upon completion
    */
   fprintf(fp, "TEST 9 - Delete all keys in rdx_data1 trie\n");
   for ( node1=0 ; node1<MAX_NUM_RDX_NODES ; node1++ )
   {
      dnp = rdx_pat_delete(&rdx_data1, rdx_data1_keys[node1]);
      if (dnp == NULL)
      {
         fprintf(fp, "Node = %4d NULL return - delete fail\n", node1);
      }
      else
      {
         fprintf(fp, "Node = %4d\n", node1);
         print_keys(fp, dnp);
      }
   }
   fprintf(fp, "rdx_data1 - Nodes allocated = %d\n", rdx_pat_nodes(&rdx_data1));

   return_code = rdx_pat_verify(&rdx_data1, ERR_CODE_PRINT, fp);
   if ( return_code != 0 )
   {
      fprintf(fp, "rdx_pat_verify() verification error in TEST 9 - %d\n", return_code);
   }
   else
   {
      fprintf(fp, "rdx_pat_verify() verification successful in TEST 9\n");
   }
   fprintf(fp, "\n\n\n");


   /*
    * TEST 10 - Insert/Search/Delete MAX_NUM_RDX_NODES nodes with random keys repeatedly.
    *           Expected Results:
    *              a. Do not report success - report only errors; insert/search/delete operation failures.
    */
   fprintf(fp, "TEST 10 - Insert/Search/Delete MAX_NUM_RDX_NODES nodes with random keys repeatedly.\n");
   for ( node1=0,tot_errs=0 ; node1<32 ; node1++ )
   {
      unsigned int n;

      for ( n=0 ; n<MAX_NUM_RDX_NODES ; n++ )
      {
         gen_keys(rdx_data1_keys[n]);
      }

      for ( n=0 ; n<MAX_NUM_RDX_NODES ; n++ )
      {
         return_code = rdx_pat_insert(&rdx_data1, rdx_data1_keys[n], &dnp);
         if ( return_code != 0 )
         {
            fprintf(fp, "rdx_pat_insert() error in TEST 9 - %d\n", return_code);
            tot_errs++;
         }
      }

      for ( n=0 ; n<MAX_NUM_RDX_NODES ; n++ )
      {
         dnp = rdx_pat_search(&rdx_data1, rdx_data1_keys[n]);
         if (dnp == NULL)
         {
            fprintf(fp, "rdx_pat_search() error in TEST 9\n");
            tot_errs++;
         }
      }

      for ( n=0 ; n<MAX_NUM_RDX_NODES ; n++ )
      {
         dnp = rdx_pat_delete(&rdx_data1, rdx_data1_keys[n]);
         if (dnp == NULL)
         {
            fprintf(fp, "rdx_pat_delete() error in TEST 9\n");
            tot_errs++;
         }
      }

      fprintf(fp, "test = %5d  tot_errs = %5d\n", node1, tot_errs);
      fflush(fp);
   }
   fprintf(fp, "TEST 10 - Total Insert/Search/Delete tests run %d  Total errors detected %d\n\n", node1, tot_errs);


   /*
    * TEST 11 - Test if NUM_KEYS(=3) keys are in the data structure but not in the same data node then search/delete for that set of keys fails.
    *           Expected Results:
    *              a. search on the two inserted data nodes should not report errors
    *              b. search on keys from both data nodes should fail
    *              c. delete on keys from both data nodes should fail
    */
   if ( NUM_KEYS == 3 && MAX_NUM_RDX_NODES >= 2 )
   {
      fprintf(fp, "TEST 11 - Test if NUM_KEYS(=3) keys are in the data structure but not in the same data node then search/delete for that set of keys fails.\n");
      for ( k=0 ; k<NUM_KEYS ; k++ )
      {
         for ( b=0 ; b<NUM_KEY_BYTES ; b++ )
         {
            rdx_data1_keys[0][k][b] = 0;
            rdx_data1_keys[1][k][b] = 0;
         }
      }
      rdx_data1_keys[0][0][NUM_KEY_BYTES-1] = 1;
      rdx_data1_keys[0][1][NUM_KEY_BYTES-1] = 2;
      rdx_data1_keys[0][2][NUM_KEY_BYTES-1] = 3;

      rdx_data1_keys[1][0][NUM_KEY_BYTES-1] = 4;
      rdx_data1_keys[1][1][NUM_KEY_BYTES-1] = 5;
      rdx_data1_keys[1][2][NUM_KEY_BYTES-1] = 6;

      return_code = rdx_pat_insert(&rdx_data1, rdx_data1_keys[0], &dnp);
      fprintf(fp, "Node = %4d Return Code = %d\n", 0, return_code);
      print_keys(fp, dnp);
      return_code = rdx_pat_insert(&rdx_data1, rdx_data1_keys[1], &dnp);
      fprintf(fp, "Node = %4d Return Code = %d\n", 1, return_code);
      print_keys(fp, dnp);

      dnp = rdx_pat_search(&rdx_data1, rdx_data1_keys[0]);
      if (dnp == NULL)
      {
         fprintf(fp, "rdx_pat_search() TEST 11 - first search fail - unexpected.\n");
      }
      else
      {
         fprintf(fp, "rdx_pat_search() TEST 11 - first search succeeds - expected.\n");
      }

      dnp = rdx_pat_search(&rdx_data1, rdx_data1_keys[1]);
      if (dnp == NULL)
      {
         fprintf(fp, "rdx_pat_search() TEST 11 - second search fail - unexpected.\n");
      }
      else
      {
         fprintf(fp, "rdx_pat_search() TEST 11 - second search succeeds - expected.\n");
      }

      rdx_data1_keys[1][0][NUM_KEY_BYTES-1] = 1;
      rdx_data1_keys[1][1][NUM_KEY_BYTES-1] = 2;
      rdx_data1_keys[1][2][NUM_KEY_BYTES-1] = 6;

      dnp = rdx_pat_search(&rdx_data1, rdx_data1_keys[1]);
      if (dnp == NULL)
      {
         fprintf(fp, "rdx_pat_search() TEST 11 - search fails - keys in different data nodes - expected.\n");
      }

      dnp = rdx_pat_delete(&rdx_data1, rdx_data1_keys[1]);
      if (dnp == NULL)
      {
         fprintf(fp, "rdx_pat_delete() TEST 11 - delete fails - keys in different data nodes - expected.\n");
      }
   }
   else
   {
      fprintf(fp, "TEST 11 - Not done - needs NUM_KEYS = 3 and MAX_NUM_RDX_NODES >= 2.\n");
   }

   return 0;
}

