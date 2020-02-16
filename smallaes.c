#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smallaes.h"

#define R 5
#define NUMERIC 65536

const unsigned char sBox[16] = { 0x6, 0xb, 0x5, 0x4, 0x2, 0xe,0x7, 0xa, 0x9, 0xd, 0xf, 0xc, 0x3, 0x1, 0x0, 0x8 };

void addRoundKey( word8 state[4][4], word8 key[4][4] )
{
	for( int i=0; i<4; i++ )
		for( int j=0; j<4; j++ )
			state[i][j] ^= key[i][j] ;
}

void subBytes( word8 state[4][4] )
{
	for( int i=0; i<4; i++ )
                for( int j=0; j<4; j++ )
			state[i][j] = sBox[ state[i][j] ];
}

void shiftRows( word8 state[4][4] )
{
	word8 temp[ 3 ] = { 0 };
       	int i, j;

	 for( i = 1; i < 4; i++)
	 {
		for( j = 0; j < i; j++ )
		temp[ j ]= state[ i ][ j ];

		for( j = 0; j < (4-i); j++ )
		state[ i ][ j ] = state[ i ][ j + i ];

		for( j = ( 4-i ); j < 4; j++)
		state[ i ][ j ] = temp[ j-4+i ];
	}
}

word8 times( word8 x )
{
	if ( x & 0x8 )
		return ( x << 1^0x3 ) & 0xf;
	else
		return ( x << 1 & 0xf );
}


word8 mul( word8 x, word8 y ) //multiplication over the domain
{
	word8 base[4] = {x} ;
	for( int i=1; i<4; i++ )
		base[i] = times( base[i-1] );

	word8 res = 0;

	for( int i =0; i<4; i++ )
		res ^= base[i] * ( y >> i & 1 );

	return res;
}


void mixColumn( word8 state[4][4] )
{
	word8 tmp[4] = {0} ;

	for( int j=0; j<4; j++ )
	{
		for( int i = 0; i<4; i++ )
			tmp[i] = state[i][j] ;

		state[0][j] = mul( 0x2, tmp[0] ) ^ mul( 0x3, tmp[1] ) ^ mul( 0x1, tmp[2] ) ^ mul( 0x1, tmp[3] );
		state[1][j] = mul( 0x1, tmp[0] ) ^ mul( 0x2, tmp[1] ) ^ mul( 0x3, tmp[2] ) ^ mul( 0x1, tmp[3] );
		state[2][j] = mul( 0x1, tmp[0] ) ^ mul( 0x1, tmp[1] ) ^ mul( 0x2, tmp[2] ) ^ mul( 0x3, tmp[3] );
      	state[3][j] = mul( 0x3, tmp[ 0 ] ) ^ mul( 0x1, tmp[ 1 ] ) ^ mul( 0x1, tmp[ 2 ] ) ^ mul( 0x2, tmp[ 3 ] );
    } 
}




void keyExpansion( word8 masterKey[4][4],word8  roundKey[11][4][4] )
{
    word8 tmp[4][44] = { 0 };

    //generate the round constant
    word8 RC[16] = { 0 };
    RC[ 1 ] = 0x1;
    for ( int i = 2; i < 16; i++ )
        RC[ i ] = times( RC[ i - 1 ] );

    for ( int j = 0; j < 4; j++ )
        for ( int i = 0; i < 4; i++ )
            tmp[i][j] = masterKey[i][j];

    for ( int j = 4; j < 44; j++ )
    {
        if ( j % 4 == 0 )
        {
            tmp[0][j] = tmp[0][j - 4] ^ sBox[ tmp[1][ j - 1 ] ] ^ RC[ j / 4 ];
            for ( int i = 1; i < 4; i++ )
                tmp[i][j] = tmp[i][j - 4 ] ^ sBox[ tmp[ (i + 1) % 4 ][ j - 1 ] ];
        }

        else 
        {
            for ( int i = 0; i < 4; i++ )
                tmp[i][j] = tmp[i][ j - 4 ] ^ tmp[i][ j - 1 ];
        }
            }

    for ( int i = 0; i < 44; i++ )
        for ( int j = 0; j < 4; j++ )
                roundKey[i / 4][j][i % 4] = tmp[j][i];
}


void encryption( word8 plaintext[4][4], word8 key[4][4], word8 ciphertext[4][4] )
{
    word8 roundKey[11][4][4] = { 0 };
    word8 tmp[4][4] = { 0 };
    //memcpy( tmp, plaintext, 16 );
    for( int i=0; i<4; i++ )
	    for( int j=0; j<4; j++ )
		    tmp[i][j] = plaintext[i][j] ;

    keyExpansion( key, roundKey );

    //white key
    addRoundKey( tmp, roundKey[0] );

    //9 same round
    for ( int i = 1; i < R; i++ )
    {
        subBytes( tmp );
        shiftRows( tmp );
        mixColumn( tmp );
        addRoundKey( tmp, roundKey[i] );
    }

    //final round
    subBytes( tmp );
    shiftRows( tmp );
    addRoundKey( tmp, roundKey[R] );

   // memcpy( ciphertext, tmp, 16 );
   for( int i=0; i<4; i++ )
	    for( int j=0; j<4; j++ )
		    ciphertext[i][j] = tmp[i][j]  % 16;
}

/*
int main ()
{
	word8 cipher[4][4];

	//secret key
	word8 key[4][4] = {
	0x00,0x04,0x08,0x0c,
	0x01,0x01,0x09,0x0d,
	0x02,0x06,0x01,0x0e,
	0x03,0x07,0x0b,0x00,};

	word8 plain[4][4] = {
		0x01,0x02,0x03,0x04,
		0x05,0x06,0x07,0x08,
		0x01,0x02,0x03,0x04,
		0x05,0x06,0x07,0x08,
	};

	encryption( plain, key, cipher );

	printf( "the ciphertext is \n");
	for (int i=0; i<4; i++ )
	{
		for( int j=0; j<4; j++ )
			printf( "  %x ", cipher[i][j] );
			printf( " \n " );
	}

	return 0;
}

*/












