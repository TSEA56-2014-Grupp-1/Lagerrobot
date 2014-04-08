#ifndef USART_H_
#define USART_H_
/**
 *	@file usart.h
 *	@author Karl Linderhed
 *
 *	Shared functions for working with UART/USART communication.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define USART_RECEIVE_TIMEOUT_COUNT 60000

void usart_init(uint16_t baudrate_register_value);
void usart_clear_buffer(void);
void usart_write_byte(uint8_t data);
uint8_t usart_read_byte(uint8_t * data);
uint8_t usart_has_bytes(void);
uint8_t usart_ready_to_write(void);
void usart_tx_frame(void);
uint8_t usart_tx_complete(void);

#endif
