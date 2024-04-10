/*
 * Copyright (c) 2023 Laurent Woolcap
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/sys_heap.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/crypto/crypto.h>
#include <zephyr/drivers/gpio.h>

#include <string.h>
#include <stdio.h>
#include "main.h"
#include "simply_aes.h"
#include "utils.h"

#ifdef CONFIG_CRYPTO_TINYCRYPT_SHIM
#define CRYPTO_DRV_NAME CONFIG_CRYPTO_TINYCRYPT_SHIM_DRV_NAME
#elif CONFIG_CRYPTO_MBEDTLS_SHIM
#define CRYPTO_DRV_NAME CONFIG_CRYPTO_MBEDTLS_SHIM_DRV_NAME
#elif DT_HAS_COMPAT_STATUS_OKAY(st_stm32_cryp)
#define CRYPTO_DEV_COMPAT st_stm32_cryp
#elif DT_HAS_COMPAT_STATUS_OKAY(st_stm32_aes)
#define CRYPTO_DEV_COMPAT st_stm32_aes
#elif DT_HAS_COMPAT_STATUS_OKAY(nxp_mcux_dcp)
#define CRYPTO_DEV_COMPAT nxp_mcux_dcp
#elif CONFIG_CRYPTO_NRF_ECB
#define CRYPTO_DEV_COMPAT nordic_nrf_ecb
#else
#error "You need to enable one crypto device"
#endif

/* change this to any other UART peripheral if desired */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

/* Message size in characters*/
#define MSG_CHAR_SIZE 512
#define MSG_BYTE_SIZE (MSG_CHAR_SIZE	>>	2)
#define MSG_WORD_SIZE (MSG_BYTE_SIZE	>>	2)

#define version "0.0.0"

/* queue to store up to 10 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uart_msgq, MSG_CHAR_SIZE, 10, 4);

static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* device tree for led */
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* receive buffer used in UART ISR callback */
static char rx_buf[MSG_CHAR_SIZE];
static int rx_buf_pos;
uint32_t cap_flags;
const struct device *dev;

/**
 * @brief header structure 
 */
struct header {
	uint8_t cla;
	uint8_t ins;
	uint8_t p1;	
	uint8_t p2;
	uint16_t length;
	union  {
		uint8_t byte[MSG_BYTE_SIZE - 6];
		uint16_t word[MSG_WORD_SIZE - 3];
	} data;
};

struct header * command;
uint8_t out[MSG_BYTE_SIZE];

/**
 * @brief swap 16bit value
 * @param val input value
 * @return uint16_t 
 */
uint16_t swap16(uint16_t val) {
    return ((val << 8) | (val >> 8 ));
}

/**
 * @brief characters from UART until line end is detected. Afterwards push the
 * data to the message queue.
 */
void read(const struct device *dev, void *user_data) {
	uint8_t c;

	if (!uart_irq_update(uart_dev)) {
		return;
	}

	if (!uart_irq_rx_ready(uart_dev)) {
		return;
	}

	/* read until FIFO empty */
	while (uart_fifo_read(uart_dev, &c, 1) == 1) {
		if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
			/* terminate string */
			rx_buf[rx_buf_pos] = '\0';

			/* if queue is full, message is silently dropped */
			k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rx_buf_pos = 0;
		} else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
			rx_buf[rx_buf_pos++] = c;
		}
		/* else: characters beyond buffer size are dropped */
	}
}

/**
 * @brief a null-terminated string character by character to the UART interface
 * @param pointer on output buffer
 */
void send(char *buf) {
	int lenght = strlen(buf);
	for (int i = 0; i < lenght; i++) {
		uart_poll_out(uart_dev, buf[i]);
	}
}

/**
 * @brief basic check of the command string, and byte convertion
 * @param in pointer on input string
 * @param out pointer on output byte array.
 */
int check(char *in, uint8_t *out) {
	// Get lenght
	int length = strlen(in);
	if ((length < 12) || (length % 2 != 0))
		return -1;
    for (int i = 0; i < length; i++)
    {
        // Reads in & stores in out
        sscanf(in, "%2hhx", &out[i]);
        in += 2;
    }
	// cast to header structure
 	command = (struct header * )out;
	// convert length
	command->length = swap16(command->length);
	// check length consistency with data received
	if (((length / 2) - command->length) != 6) {
		return -2;
	}
	return 0;
}

/**
 * @brief while loop command
 * @param iteration
 */
void while_loop(uint16_t iteration) {
	uint16_t index, count = 0;
	char result[24];
	index = iteration;

	trigger_high();
	while(index) {
		index--;
		count++;
	}
	trigger_low();
	sprintf(result, "%04X%04X9000\r\n",index,count);
	send(result);
}

/**
 * @brief for loop command
 * @param iteration
 */
void for_loop(uint16_t iteration) {
	char result[24];
	uint16_t index, count = 0;

	trigger_high();
	for (index = 0; index < iteration; index++) {
		count ++;
	}
	trigger_low();

	sprintf(result, "%04X%04X9000\r\n",index,count);
	send(result);
}

/**
 * @brief encrypt with simply aes command
 * @param pointer on data in
 */
void simply_aes(uint8_t * data) {
	uint8_t result[2 * 16 + 4];
	uint8_t offset = 0;
	simple_aes_init(data);
	
	trigger_high();
	simple_aes_cipher();
	trigger_low();
	
	simple_aes_final(data);

	for (int i=0; i < 16; i++) {
		offset += sprintf(result + offset, "%02X", data[i]);
	}
	sprintf(result + offset, "9000\r\n");
	send(result);
}

/**
 *  @brief
 */
int validate_hw_compatibility(const struct device *dev)
{
	uint32_t flags = 0U;

	flags = crypto_query_hwcaps(dev);
	if ((flags & CAP_RAW_KEY) == 0U) {
		printk("Please provision the key separately as the module doesnt support a raw key");
		return -1;
	}

	if ((flags & CAP_SYNC_OPS) == 0U) {
		printk("The app assumes sync semantics. Please rewrite the app accordingly before proceeding");
		return -1;
	}

	if ((flags & CAP_SEPARATE_IO_BUFS) == 0U) {
		printk("The app assumes distinct IO buffers. Please rewrite the app accordingly before proceeding");
		return -1;
	}

	cap_flags = CAP_RAW_KEY | CAP_SYNC_OPS | CAP_SEPARATE_IO_BUFS;

	return 0;

}

/**
 * @brief aes encrypt ecb mode
 * @param data pointer
 */
void encrypt_aes_ebc(uint8_t * data) {
	uint8_t result[2 * 16 + 6];
	uint8_t offset = 0;
	uint8_t encrypted[16] = {0};

	struct cipher_ctx ini = {
		.keylen = 16,
		.key.bit_stream = data,
		.flags = cap_flags,
	};
	struct cipher_pkt encrypt = {
		.in_buf = &data[16],
		.in_len = 16,
		.out_buf_max = sizeof(encrypted),
		.out_buf = encrypted,
	};
	if (cipher_begin_session(dev, &ini, CRYPTO_CIPHER_ALGO_AES,
				 CRYPTO_CIPHER_MODE_ECB,
				 CRYPTO_CIPHER_OP_ENCRYPT)) {
		return;
	}
	trigger_high();
	if (cipher_block_op(&ini, &encrypt)) {
		goto out;
	}
	trigger_low();
	for (int i=0; i < 16; i++) {
		offset += sprintf(result + offset, "%02X", encrypt.out_buf[i]);
	}
	sprintf(result + offset, "9000\r\n");
	send(result);
out:
	cipher_free_session(dev, &ini);	
}

/**
 * @brief Dispatch command
 * @param pointer to input buffer
 */
int dispatch(char *in) {
	int status = 0;
	// start watchdog

	// check input data
	status = check(in,out);
	
	if (status==0) {
		if (command->cla != CLASS)
			send("6E00\r\n");
		else {
			switch (command->ins) {
				case INS_GET_INFO:
					printk("Chip evaluation firmware board= %s, version=%s\r\n", CONFIG_BOARD,version);
					break; 

				case INS_LOOP:
					if (command->length != 2)
						send("6700\r\n");
					else {
						if (command->p1==LOOP_FOR) {
							// call for loop process
							for_loop(command->data.word[0]);
						} else if (command->p1==LOOP_WHILE) {
							// call while loop process
							while_loop(command->data.word[0]);
						} else {
							send("6A00\r\n");
						}
					}
					break;
				case INS_CRYPTO:
					if (command->length != 32)
						send("6700\r\n");
					else
						if (command->p1==SIMPLY_ENCRYPT_AES_ECB) {
							simply_aes(command->data.byte);							
						} else if (command->p1==DRIVER_ENCRYPT_AES_ECB) {
							encrypt_aes_ebc(command->data.byte);
						} else {
							send("6A00\r\n");
						}
					break;

				default:
					send("6986\r\n");
					break;
			}
		}
	}
	if (status==-1) {
		send("6F01\r\n");
	}

	if (status==-2) {
		send("6700\r\n");
	}
	// stop watchdog
	
	return 0;
}

/**
 * 
 */
int main(void)
{
	char cmd_buf[MSG_CHAR_SIZE];

#ifdef CRYPTO_DRV_NAME
	dev = device_get_binding(CRYPTO_DRV_NAME);
	if (!dev) {
		printk("%s pseudo device not found", CRYPTO_DRV_NAME);
		return 0;
	}
#else
	dev = DEVICE_DT_GET_ONE(CRYPTO_DEV_COMPAT);

	if (!device_is_ready(dev)) {
		printk("Crypto device is not ready\n");
		return 0;
	}
#endif

	if (!device_is_ready(uart_dev)) {
		printk("UART device not found!");
		return 0;
	}
	if (validate_hw_compatibility(dev)) {
		printk("Incompatible crypto h/w");
		return 0;
	}
	/* configure interrupt and callback to receive data */
	int ret = uart_irq_callback_user_data_set(uart_dev, read, NULL);

	if (ret < 0) {
		if (ret == -ENOTSUP) {
			printk("Interrupt-driven UART API support not enabled\n");
		} else if (ret == -ENOSYS) {
			printk("UART device does not support interrupt-driven API\n");
		} else {
			printk("Error setting UART callback: %d\n", ret);
		}
		return 0;
	}

	uart_irq_rx_enable(uart_dev);

	/* Set led0 */
	if (!gpio_is_ready_dt(&led)) {
		printk("Error indicator not ready...");
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Error indicator configuration...");
		return 0;
	}

	ret = gpio_pin_set_dt(&led, 1);
	if (ret < 0) {
		printk("Error indicator toggle...");
		return 0;
	}

	ret = trigger_init();
	if (ret < 0) {
		printk("Error trigger init...");
		return 0;
	}

	/* indefinitely wait for input from the user */
	while (k_msgq_get(&uart_msgq, &cmd_buf, K_FOREVER) == 0) {	
		dispatch(cmd_buf);
	}
	return 0;
}
