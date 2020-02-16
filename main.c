#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"core.h"

int main()
{
    printf( "Start Recover K01 ^ K30...\n" );
//word8 core( word8 z0, word8 z1, word8 DELTA[], word8 key[4][4] ) // for encryption
    word8 key[4][4];
//void genSubspaceFirstDiagonal( int num, word8 subspace[][4] )
    genSubspaceFirstDiagonal( DATANUM, subspace );

    srand( time(NULL) );
    for ( int i = 0; i < 4; i++ )
        for ( int j = 0; j < 4; j++ )
            key[i][j] = rand() % NIBBLE;

    printf( "Right: %x\n", key[0][1] ^ key[3][0] );

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
    printf("Recvr: " );
    for ( int i = 0; i < 16; i++ )
        if ( RESULT[i] < 16 )
            printf("%x\t", RESULT[i] );
    printf("\n" );
}
