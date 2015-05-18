#ifndef _RDX_PAT_SEARCH_H
#define _RDX_PAT_SEARCH_H

/*
 * header file for rdx_pat_search.c radix PATRICIA search trie routines.
 * to be included in application code.
 */

/* includes application defined data structure typedef APP_DATA */
#include "rdx_pat_data.h"

/* number of rdx search keys */
#define NUM_KEYS           3

/* maximum number of data nodes stored in rdx trie */
#define MAX_NUM_RDX_NODES  8

/* number of bytes in each key(s) */
#define NUM_KEY_BYTES      4

/* verify mode arg to rdx_pat_verify() */
typedef enum verify_mode
{
    ERR_CODE,              /* return 0 for no error and positive int with error code for any errors */
    ERR_CODE_PRINT         /* return same error code above but also print current data structure data
                              node addresses/keys, branch node addresses and error messages */
} VERIFY_MODE;

/* branch node typedef */
typedef struct bnode
{
    unsigned int id;  /* IDentification(id=0) for branch node - must be first field in node */
    unsigned int br;  /* br=0: from left parent BRanch - br=1: from right parent branch */
    void *p;          /* ptr to Parent node */
    unsigned int nsn; /* Node Sequence Number - 0->MAX_NUM_RDX_NODES */
    unsigned int b;   /* Bit number to do branch test on(bits start at 0 from right to left) */
    void *l, *r;      /* Left and Right node ptrs */
} BNODE;

/*
 * data node typedef for application declarations.
 * the key must have one extra byte from NUM_KEY_BYTES
 * specified above in order to have an "impossible"
 * key data node set at initialization 
 */
typedef struct dnode
{
    unsigned int id;           /* IDentification(id=1) for data node - must be first field in node */
    unsigned int br[NUM_KEYS]; /* br=0: from left parent BRanch - br=1: from right parent branch */
    BNODE *p[NUM_KEYS];        /* ptr to Parent node */
    unsigned int nsn;          /* Node Sequence Number - 0->MAX_NUM_RDX_NODES */
    void *nnfp;                /* Next Node Free Ptr */
    unsigned int alloc;        /* 1 - ALLOCated in rdx trie, 0 - on free queue */
    unsigned char key[NUM_KEYS][NUM_KEY_BYTES+1]; /* search KEY(s) */
    APP_DATA data;             /* user defined data structure */
} DNODE;

/* typedef of struct for PATRICIA node holding MAX_NUM_RDX_NODES data nodes with NUM_KEYS keys of length NUM_KEY_BYTES */
typedef struct pnode
{
    /* total number of nodes alloc(does not include root(0xff key) node) */
    unsigned int tot_nodes;

    /* head of rdx search node trie for each key */
    BNODE *head[NUM_KEYS];

    /*
     * array holding ptrs to DNODES used in rdx_pat_sort().  initialized to all 0xf0 which should not affect
     * anything, but might be useful for debugging(the last element points to the root node with key 0xff)
     */
    DNODE *node_ptrs[MAX_NUM_RDX_NODES+1];

    /* number of ptrs in node_ptrs array(not including root node) */
    unsigned int node_ptrs_cnt;

    /* branch nodes and free queue head ptr(s) of branch nodes */
    BNODE bnodes[MAX_NUM_RDX_NODES+1][NUM_KEYS];
    BNODE *bfree_head[NUM_KEYS];

    /* data nodes and free queue head ptr of data nodes */
    DNODE dnodes[MAX_NUM_RDX_NODES+1];
    DNODE *dfree_head;
} PNODE;


/* function declarations */

    unsigned int
rdx_pat_initialize
    (
        PNODE *pnodep
    );

    int
rdx_pat_insert
    (
        PNODE *pnodep,
        unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES],
        DNODE **return_ptr
    );

    DNODE *
rdx_pat_search
    (
        PNODE *pnodep,
        unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
    );

    DNODE *
rdx_pat_delete
    (
        PNODE *pnodep,
        unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES]
    );

    int
rdx_pat_sort
    (
        PNODE *pnodep,
        DNODE ***data_nodep,
        unsigned int k
    );

    int
rdx_pat_nodes
    (
        PNODE *pnodep
    );

    int
rdx_pat_print
    (
        PNODE *pnodep,
        unsigned char key[NUM_KEYS][1+NUM_KEY_BYTES],
        FILE *fp
    );

    int
rdx_pat_verify
    (
        PNODE *pnodep,
        VERIFY_MODE vm,
        FILE *fp
    );

#endif /* _RDX_PAT_SEARCH_H */
