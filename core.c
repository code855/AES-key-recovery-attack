#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include"smallaes.h"
#include"core.h"

/*
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
*/

void printM( word8 x[4][4] )
{
    for ( int i = 0; i < 4; i++ )
        printf( "%x %x %x %x\n", x[i][0], x[i][1], x[i][2], x[i][3] );
}

void int2array( word16 x, word8 y[4] )
{
    y[0] = x >> 12 & 0xf;
    y[1] = x >> 8 & 0xf;
    y[2] = x >> 4 & 0xf;
    y[3] = x >> 0 & 0xf;}

word16 array2int( word8 y[4] )
{
    return ( y[0] << 12 )  ^ ( y[1] << 8 ) ^ ( y[2] << 4 ) ^ y[3];
}

word16 getDiagonal( word8 x[4][4], int d )
{
    word8 y[4] = { x[0][d], x[1][(d + 1) % 4], x[2][(d + 2) % 4], x[3][(d + 3) % 4] };
    return array2int(y);
}

word16 getInvDiagonal( word8 x[4][4], int d )
{
    word8 y[4] = { x[0][d], x[1][(d + 4 - 1) % 4], x[2][(d + 4 - 2) % 4], x[3][(d + 4 - 3) % 4] };
    return array2int(y);
}
/* generate the subspace of the first diagonal containing num x \in (F_2^4)^4 */
void genSubspaceFirstDiagonal( int num, word8 subspace[][4] )
{
    // for a convenient comparision, we first use the set containing x \in F_2^16
    word16 tmp[WORD] = { 0 }; 

    srand( time( NULL ) ); 
    for( int i = 0; i < num; )
    {
        int j;
        tmp[i] = rand() % WORD; 
        for ( j = 0; j < i; j++ ) 
        {
            if ( tmp[j] == tmp[i] ) // find a replicated value
                break;
        }
        if ( j == i ) // no replicated value
            i++;
    }
    for ( int i = 0; i < num; i++ )
        int2array( tmp[i], subspace[i] );
}

void generateAz( word8 delta, word8 z0, word8 z1, int num, word8 subspace[][4], word8 Adz0[][4][4], word8 Adz1[][4][4] )
{
    srand( time ( NULL ) );
    word8 p[4][4];
    for (int i = 0; i < 4; i++ )
        for ( int j = 0; j < 4; j++ )
            p[i][j] = rand() % NIBBLE;
    //generate Adz0
    p[0][1] = z0; p[3][0] = z0 ^ delta;
    for ( int i = 0; i < num; i++ )
    {
        for ( int j = 0; j < 4; j++ )
            for ( int k = 0; k < 4; k++ )
                Adz0[i][j][k] = p[j][k];

        Adz0[i][0][0] = subspace[i][0];
        Adz0[i][1][1] = subspace[i][1];
        Adz0[i][2][2] = subspace[i][2];
        Adz0[i][3][3] = subspace[i][3];
    }

    //generate Adz1
    p[0][1] = z1; p[3][0] = z1 ^ delta;
    for ( int i = 0; i < num; i++ )
    {
        for ( int j = 0; j < 4; j++ )
            for ( int k = 0; k < 4; k++ )
                Adz1[i][j][k] = p[j][k];

        Adz1[i][0][0] = subspace[i][0];
        Adz1[i][1][1] = subspace[i][1];
        Adz1[i][2][2] = subspace[i][2];
        Adz1[i][3][3] = subspace[i][3];
    }
}

void init( int array[][COLLISION], int num ) 
{
    for ( int i = 0; i < num; i++ )
        for ( int j = 0; j < COLLISION; j++ )
            array[i][j] = -1;
}

void core( word8 z0, word8 z1, word8 subspace[][4],
            word8 DELTA[], word8 key[4][4] ) // for encryption
{
    int pdelta = 0;

    // allocate Cz0, Cz1
    // for all possible delta
    for ( int delta = 0; delta < 16; delta++ )
    {
nextDelta:

        // prepare Adz0, Adz1
        generateAz( delta, z0, z1, DATANUM, subspace, Adz0, Adz1 );

        // encrypt Adz0 and Adz1
        word8 c0[4][4] = { 0 }, c1[4][4] = { 0 };
        for ( int i = 0; i < DATANUM; i++ )
        {
            encryption( Adz0[i], key, c0 );
            encryption( Adz1[i], key, c1 );

            // insert c0 c1 into Cz0 Cz1
            for ( int j = 0; j < 4; j++ )
                for ( int k = 0; k < 4; k++ )
                {
                    Cz0[i][j][k] = c0[j][k];
                    Cz1[i][j][k] = c1[j][k];
                }
         }
         // for each inverse diagonal, find collision pair
         for ( int diag = 0; diag < 4; diag++ )
         {
            // allocate 2 hash table
            init( Tz0, WORD ); init( Tz1, WORD ); 
            for ( int i = 0; i < DATANUM; i++ )
            {
                word16 invz0 = getInvDiagonal( Cz0[i],  diag );
                word16 invz1 = getInvDiagonal( Cz1[i],  diag );
                for ( int ii = 0; ii < COLLISION; ii++ )
                    if ( Tz0[invz0][ii] == -1 )
                    {
                        Tz0[invz0][ii] = i;
                        break;
                    }
                for ( int ii = 0; ii < COLLISION; ii++ )
                    if ( Tz1[invz0][ii] == -1 )
                    {
                        Tz1[invz1][ii] = i;
                        break;
                    }
            }
                // find collision pairs
                // every line
            for ( int i = 0; i < WORD; i++ )
            {
                int j1 = 0, j2 = 0;
                for ( j1 = 0; j1 < COLLISION; j1++ ) 
                    if ( Tz0[i][j1] == -1 )
                        break;
                for ( j2 = 0; j2 < COLLISION; j2++ ) 
                    if ( Tz1[i][j2] == -1 )
                        break;
                // collision pairs 
                for ( int j = 0; j < j1; j++ )
                    for ( int k = 0; k < j2; k++ )
                    {
                        if ( Tz0[i][j] == Tz1[i][j] ) // equal diagonals
                            continue;
                        if ( getInvDiagonal( Cz1[Tz0[i][j]], diag ) == getInvDiagonal( Cz0[Tz1[i][k]], diag ) )
                        {
                            DELTA[pdelta++] = delta;
                            delta++;
                            goto nextDelta;
                        }
                    }
            }
         }
    }
}

void Remove( word8 DELTA [], int num, word8 xor )
{
    for ( int i = 0; i < num; i++ )
    {
        word8 candidate = DELTA[i] ^ xor;
        int flag = 0;
        for ( int j = 0; j < num; j++ )
            if ( DELTA[j] == candidate )
                flag = 1;
        if ( flag == 0 )
            DELTA[i] = 255;
    }
}
/*
int main()
{
    printf( "Start...\n" );
//word8 core( word8 z0, word8 z1, word8 DELTA[], word8 key[4][4] ) // for encryption
    word8 key[4][4];
//void genSubspaceFirstDiagonal( int num, word8 subspace[][4] )
    genSubspaceFirstDiagonal( DATANUM, subspace );

    srand( time(NULL) );
    for ( int i = 0; i < 4; i++ )
        for ( int j = 0; j < 4; j++ )
            key[i][j] = rand() % NIBBLE;

    printf( "The right Key byte: %x\n", key[0][1] ^ key[3][0] );

    word8 RESULT[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf}; 

    for ( int test = 0; test < TESTNUM; test++ )
    {
        word8 z0, z1;
        do
        {
            z0 = rand() % NIBBLE;
            z1 = rand() % NIBBLE;
        } while ( z0 == z1 );

        word8 DELTA[16];
        for ( int i = 0; i < 16; i++ )
            DELTA[i] = 255;

        core( z0, z1, subspace, DELTA, key );

        int num = 0;
        for ( num = 0; num < 16; num++ )
            if ( DELTA[num] >= 16 )
                break;
        Remove( DELTA, num, z0 ^ z1 );

        for ( int i = 0; i < 16; i++ )
        {
            if ( RESULT[i] < 16 )
            {
                int flag = 0;
                for ( int j = 0; j < num; j++ )
                    if ( RESULT[i] == DELTA[j] )
                        flag = 1;
                if ( flag == 0 )
                    RESULT[i] = 255;
             }
        }
   }
   for ( int i = 0; i < 16; i++ )
       if ( RESULT[i] < 16 )
            printf("%x\t", RESULT[i] );
   printf("\n");

}
*/

