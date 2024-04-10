#define CLASS                   0xFE /*!< Command Class byte */

#define INS_GET_INFO            0x80 /*!< Get info instruction byte */

#define INS_LOOP                0x86 /*!< Loop Instruction byte */
#define LOOP_FOR                0x02 /*!< P1 Parameter: for loop */
#define LOOP_WHILE              0x04 /*!< P1 Parameter: while loop */

#define INS_CRYPTO              0x8A
#define SIMPLY_ENCRYPT_AES_ECB  0x02
#define DRIVER_ENCRYPT_AES_ECB  0x04
