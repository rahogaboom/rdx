
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

//#define DEBUG
#include "MKRdxPat.h"

using namespace MultiKeyRdxPat;
using namespace std;


   int
main()
{

    int num_keys = 3;
    int num_key_bytes = 4;
    int rc;
    int nodes = -1;

    unsigned char keys[3][1+4];
    unsigned char *key = (unsigned char *)keys;

    DNODE *dnodep;

    FILE *fp;

    fp = fopen("MKRdxPat-test.results", "w");

    cout << "##########################################################" << endl;
    cout << "Test 1" << endl;
    MKRdxPat *rdx0 = new MKRdxPat( 512, 3, 4 );

    memset( key, 0, num_keys*(num_key_bytes+1));

    keys[0][num_key_bytes] = 1;
    keys[1][num_key_bytes] = 2;
    keys[2][num_key_bytes] = 3;

    keys[0][0] = 1;
    keys[1][0] = 1;
    keys[2][0] = 1;

    for ( int i=0 ; i<num_keys ; i++ )
    {
        for ( int j=0 ; j<num_key_bytes+1 ; j++ )
        {
            printf("%X ", keys[i][j]);
        }
        printf("\n");
    }

    for ( int i=0 ; i<num_keys*(num_key_bytes+1) ; i++ )
    {
        printf("%X ", key[i]);
    }
    printf("\n");

    /*
     * insert()
     */
    cout << "insert()" << endl;

    nodes = rdx0->nodes();
    cout << "nodes = " << nodes << endl;

    rc = rdx0->insert( key, &dnodep );
    cout << "Test 1 - insert() rc: " << rc << endl;

    dnodep->data.id = 1000;
    cout << "dnodep->data.id = " << dnodep->data.id  << endl;

    nodes = rdx0->nodes();
    cout << "nodes = " << nodes << endl;


    /*
     * search()
     */
    cout << "search()" << endl;

    //dnodep = rdx0->search( key );
    cout << "dnodep->data.id = " << dnodep->data.id  << endl;


    /*
     * sort()
     */
    cout << "sort()" << endl;

    DNODE **sorted_nodes;

    rc = rdx0->sort( &sorted_nodes, 1);
    cout << "Test 1 - sort() rc: " << rc << endl;
    cout << ((DNODE *)sorted_nodes[0])->data.id << endl;

    /*
     * print()
     */
    cout << "print(key, fp)" << endl;
    rdx0->print( key, fp );

    cout << "print(NULL, fp)" << endl;
    rdx0->print( NULL, fp );

    /*
     * remove()
     */
    cout << "remove()" << endl;

    rdx0->remove( key );

    nodes = rdx0->nodes();
    cout << "nodes = " << nodes << endl;

    /*
     * verify()
     */
    cout << "verify()" << endl;

    rc = rdx0->verify(ERR_CODE, fp);
    cout << "rc = " << rc << endl;

    /*
     * delete object
     */
    delete rdx0;


    /*
     * Test 2
     */
    cout << "##########################################################" << endl;
    cout << "Test 2" << endl;
    MKRdxPat *rdx1 = new MKRdxPat( 1024, 2, 16 );

    delete rdx1;
}

