/**
 * @file main.h
 * @author Laurent Bonnet (laurent.woolcap@free.fr)
 * @brief Chip evaluation firmware, main interface file
 * @copyright Copyright (c) 2024
 */
#define CLASS                   0xFE /*!< Command Class byte */

#define INS_GET_INFO            0x80 /*!< Get info instruction byte */

/**
 * @brief loop instructions
 */
#define INS_LOOP                0x86 /*!< Loop Instruction byte */
#define LOOP_FOR                0x02 /*!< P1 Parameter: for loop */
#define LOOP_WHILE              0x04 /*!< P1 Parameter: while loop */

/**
 * @brief Cryptographic instructions
 */
#define INS_CRYPTO              0x8A /*!< Crypto Instruction byte */
#define SIMPLY_ENCRYPT_AES_ECB  0x02 /*!< AES ECB SIMPLY */
#define DRIVER_ENCRYPT_AES_ECB  0x04 /*!< AES ECB MBEDTLS */


