/* test routine for gbit() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* number of bytes in each key(s) */
#define NUM_KEY_BYTES         25

/* given a key and bit number (bits numbered 0 from right) will return that bit */
   static unsigned int
gbit(
   unsigned char *key,
   unsigned int bit_num)
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


   int
main()
{
   static unsigned int byte, bit;
   static unsigned char key[NUM_KEY_BYTES+1];
   static FILE *fp;


   fp = fopen("test_gbit.results", "w");

   key[0]  = 0x00;
   for ( byte=1 ; byte<NUM_KEY_BYTES+1 ; byte++ )
   {
      key[byte]  = 0xaa;
   }

   fprintf(fp, "Should be alternating 0's and 1's - start with 0 - the last 8 bits should all be 0(the prefix byte)\n");
   fprintf(fp, "bit index   bit\n");
   for ( bit=0 ; bit<(NUM_KEY_BYTES+1)*8 ; bit++ )
   {
      fprintf(fp, "%9d   %3d\n", bit, gbit(key, bit));
   }
   fprintf(fp, "\n\n\n");

   key[0]  = 0x00;
   for ( byte=1 ; byte<NUM_KEY_BYTES+1 ; byte++ )
   {
      key[byte]  = 0xff;
   }

   fprintf(fp, "Should be all 1's - the last 8 bits should all be 0(the prefix byte)\n");
   fprintf(fp, "bit index   bit\n");
   for ( bit=0 ; bit<(NUM_KEY_BYTES+1)*8 ; bit++ )
   {
      fprintf(fp, "%9d   %3d\n", bit, gbit(key, bit));
   }
   fprintf(fp, "\n\n\n");

   key[0]  = 0x00;
   for ( byte=1 ; byte<NUM_KEY_BYTES+1 ; byte++ )
   {
      key[byte]  = 0x00;
   }

   fprintf(fp, "Should be all 0's - the last 8 bits should all be 0(the prefix byte)\n");
   fprintf(fp, "bit index   bit\n");
   for ( bit=0 ; bit<(NUM_KEY_BYTES+1)*8 ; bit++ )
   {
      fprintf(fp, "%9d   %3d\n", bit, gbit(key, bit));
   }
}

