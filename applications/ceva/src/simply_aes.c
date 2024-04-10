/**
 * @file ul_aes.c
 * @author laurent bonnet (laurent.woolcap@free.fr)
 * @brief Chip Evaluation firmware, Simple software AES
 * @copyright Copyright (c) 2023
 */
#define __UL_AES_C
#include "simply_aes.h"
#include "utils.h"

static  const int Sbox    [256] = _Sub;
const int xtimes  [256] = _xt;
static  const int rcon[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};
int rk[176];
int state[16];
int tmp[2];
int *pt_rk;

/**
 * @brief key expansion
 */
static void key_expansion(void)
{
  int i;
  for(i=16;i<176;i+=4){
    
    if ((i&0xF)==0){
      state[0] = Sbox[rk[i-3]] ^ rcon[(i/16)-1];
      state[1] = Sbox[rk[i-2]];
      state[2] = Sbox[rk[i-1]];
      state[3] = Sbox[rk[i-4]];
    }
    
    else{
      state[0] = rk[i-4];
      state[1] = rk[i-3];
      state[2] = rk[i-2];
      state[3] = rk[i-1];
    }
    
    rk[i  ] = rk[i-16] ^ state[0];
    rk[i+1] = rk[i-15] ^ state[1];
    rk[i+2] = rk[i-14] ^ state[2];
    rk[i+3] = rk[i-13] ^ state[3];
  }
}

/**
 * @brief Add round 
 */
static void AddRoundKey(void)
{
  int j;
  for(j=0;j<16;j++){
    state[j] ^= pt_rk[j];
  }
}

/**
 * @brief 
 */
static void SubBytes_ShiftRows(void)
{
  state[ 0] = Sbox[state[ 0]];
  state[ 4] = Sbox[state[ 4]];
  state[ 8] = Sbox[state[ 8]];
  state[12] = Sbox[state[12]];

  tmp[0]    = Sbox[state[ 1]];
  state[ 1] = Sbox[state[ 5]];
  state[ 5] = Sbox[state[ 9]];
  state[ 9] = Sbox[state[13]];
  state[13] = tmp[0];
    
  tmp[0]    = Sbox[state[ 2]];
  state[ 2] = Sbox[state[10]];
  state[10] = tmp[0];

  tmp[0]    = Sbox[state[6]];
  state[ 6] = Sbox[state[14]];
  state[14] = tmp[0];

  tmp[0]    = Sbox[state[15]];
  state[15] = Sbox[state[11]];
  state[11] = Sbox[state[ 7]];
  state[7 ] = Sbox[state[ 3]];
  state[3 ] = tmp[0];
}

/**
 * @brief 
 */
static void aes_round( void )
{
  int column[4]; // temporary variable to hold a column from the state

  pt_rk += 16;
  
  //SubBytes + ShiftRows
  SubBytes_ShiftRows();
  
  //MixColumns
  
  //column 0
  column[0] = state[0];
  column[1] = state[1];
  column[2] = state[2];
  column[3] = state[3];

  state[ 0] = mix_column(0,1,2,3);
  state[ 1] = mix_column(1,2,3,0);
  state[ 2] = mix_column(2,3,0,1);
  state[ 3] = mix_column(3,0,1,2);

  //column 1
  column[0] = state[4];
  column[1] = state[5];
  column[2] = state[6];
  column[3] = state[7];

  state[ 4] = mix_column(0,1,2,3);
  state[ 5] = mix_column(1,2,3,0);
  state[ 6] = mix_column(2,3,0,1);
  state[ 7] = mix_column(3,0,1,2);

  //column 2
  column[0] = state[ 8];
  column[1] = state[ 9];
  column[2] = state[10];
  column[3] = state[11];

  state[ 8] = mix_column(0,1,2,3);
  state[ 9] = mix_column(1,2,3,0);
  state[10] = mix_column(2,3,0,1);
  state[11] = mix_column(3,0,1,2);

  //column 3
  column[0] = state[12];
  column[1] = state[13];
  column[2] = state[14];
  column[3] = state[15];

  state[12] = mix_column(0,1,2,3);
  state[13] = mix_column(1,2,3,0);
  state[14] = mix_column(2,3,0,1);
  state[15] = mix_column(3,0,1,2);

  //AddRoundKey
  AddRoundKey();
}

/**
 * @brief AES Cipher
 */
void simple_aes_cipher()
{
  int i = 0;
  int ret = 0;

  //AddRoundKey
  pt_rk = rk;
  AddRoundKey();

  for(i=0; i<9; i++){
    if (i==8) {
      ret = sub_trigger_high();
      if (ret < 0) 
        return;
    }
    aes_round();
    if (i==8) {
      ret = sub_trigger_low();
      if (ret < 0) 
        return;
    }
  }
  
  //Last round 
  SubBytes_ShiftRows(); 
  pt_rk += 16;
  AddRoundKey();
}

/**
 * @brief AES initialization process
 * @param buffer pointer on input data
 */
 void simple_aes_init(uint8_t * buffer) {
    int i,j;
    for(i=0; i<16; i++){
        rk[i] = buffer[i];
    }

    key_expansion();

    pt_rk = rk;

    for(j=0; j<16; j++,i++){
        state[j] = buffer[i];
    }
}

/**
 * @brief AES final
 * @param result Pointer on result buffer
 */
void simple_aes_final(uint8_t * result) {
    uint8_t i;
    for (i=0;i<16;i++)
    {
      result[i]=state[i];
    }
}
