#ifndef __CORE_H__
#define __CORE_H__

typedef unsigned char word8;
typedef unsigned int word16;

#define NIBBLE 16 // for a nibble 2^4
#define WORD 65536 // for a word of 2^16
#define DATANUM 32768
#define COLLISION 16
#define TESTNUM  11

static int Tz0[WORD][COLLISION] = { 0 }, Tz1[WORD][COLLISION] = { 0 };
static word8 Adz0[DATANUM][4][4] = { 0 }, Adz1[DATANUM][4][4] = { 0 };
static word8 Cz0[DATANUM][4][4] = { 0 }, Cz1[DATANUM][4][4] = { 0 };
static word8 subspace[DATANUM][4] = { 0 };

void genSubspaceFirstDiagonal( int num, word8 subspace[][4] );
void core( word8 z0, word8 z1, word8 subspace[][4],
            word8 DELTA[], word8 key[4][4] ); // for encryption
void Remove( word8 Delta[], int num, word8 XOR );
#endif

