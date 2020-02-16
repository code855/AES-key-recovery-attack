#ifndef _SMALLAES_H_
#define _SMALLAES_H_

#include<inttypes.h>
typedef unsigned char word8;

//inverse s-box
void encryption( word8 plaintext[4][4], word8 key[4][4], word8 chiphertext[4][4] );
//void decryption

void addRoundKey( word8 state[4][4], word8 key[4][4] );
void subBytes( word8 state[4][4] );
void shiftRow( word8 state[4][4] );
word8 times( word8 x );
word8 mul( word8 x, word8 y);
void mixColumn( word8 state[4][4] );

void keyExpansion( word8 masterkey[4][4], word8 roundkey[11][4][4] );

#endif

