#ifndef RDX_PAT_DATA_H
#define RDX_PAT_DATA_H

/*
 * application defined data is set here.  this file is included in MKRdxPat.h.
 * key storage is not handled by the user, but is allocated in the PNODE struct.
 */

// any user data structure of arbitrary complexity
typedef struct _app_data {
   // data stuff goes here
   int id;
   char name[64];

   struct
   {
      double d;
      char data1_id[16];
      int data1[10];

      struct
      {
         float f;
      } data1_a;
   } data1;

   struct
   {
      int id;
      char data2_id[16];
   } data2;
} APP_DATA;

#endif // RDX_PAT_DATA_H
