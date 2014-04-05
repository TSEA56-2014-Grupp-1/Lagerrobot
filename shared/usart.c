/**
 *	@file usart.c
 *	@author Karl Linderhed & Andreas Runfalk
 *
 *	Shared functions for working with UART/USART communication.
 */

#include "usart.h"

uint8_t usart_receive_buffer[256];
uint8_t usart_buffer_read_index = 0;
uint8_t usart_buffer_write_index = 0;

/**
 *	Interrupt vector to add received data to ring buffer
 */
ISR(USART0_RX_vect) {
	usart_receive_buffer[usart_buffer_write_index++] = UDR0;
}

/**
 *	Enable transmission and receiving of USART communication on PD0 and PD1
 *
 *	@param baudrate_register_value Baudrade value to put into UBRR0. Depends on
 *	                               CPU speed
 */
void usart_init(uint16_t baudrate_register_value) {
	UBRR0L = (uint8_t) baudrate_register_value;
	UBRR0H = (uint8_t) (baudrate_register_value >> 8);

	UCSR0B |= (1 << RXCIE0) | (1 << TXEN0) | (1 << RXEN0);

	// Enable pull-up for PD0 and PD1
	PORTD |= 0x03;

	sei();
}

/**
 *	Throw away all data currently in the ring buffer
 */
void usart_clear_buffer(void) {
	usart_buffer_read_index = 0;
	usart_buffer_write_index = 0;
}

/**
 *	Write one byte of data to PD1
 *
 *	@param data Byte to write
 */
void usart_write_byte(uint8_t data) {
	while (!usart_ready_to_write());
	UDR0 = data;
}

/**
 *	Read one byte of data from PD0
 *
 *	@param[out] data Pointer to write data to
 *
 *	@return 1 if read timed out or 0 if successful
 */
uint8_t usart_read_byte(uint8_t * data) {
	uint16_t timeout_counter = 0;

	while (!usart_has_bytes()) {
		if (timeout_counter++ >= USART_RECEIVE_TIMEOUT_COUNT)
			return 1;
	}

	*data = usart_receive_buffer[usart_buffer_read_index++];
	return 0;
}

/**
 *	Determines if there are unread bytes in the buffer
 *
 *	@return 1 if there are bytes to be read from buffer else 0
 */
uint8_t usart_has_bytes(void) {
	return usart_buffer_read_index != usart_buffer_write_index;
}

/**
 *	Determines if writing can be done
 *
 *	@return Positive value if ready to write else 0
 */
uint8_t usart_ready_to_write(void) {
	return UCSR0A & (1 << UDRE0);
}

/**
 *	Mark that we have completed shifting of bytes into send buffer. Used so
 *	usart_tx_complete() will know when all bytes have been sent
 */
void usart_tx_frame(void) {
	UCSR0A |= (1 << TXC0);
}

/**
 *	Determines if transfer frame is complete
 *
 *	@return Positive value if all bytes were sent else 0
 */
uint8_t usart_tx_complete(void) {
	return UCSR0A & (1 << TXC0);
}
