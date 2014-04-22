#include "usart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t usart_receive_buffer[256];
uint8_t usart_buffer_read_index = 0;
uint8_t usart_buffer_write_index = 0;

ISR(USART0_RX_vect) {
	usart_receive_buffer[usart_buffer_write_index++] = UDR0;
}

void usart_init(uint16_t baudrate_register_value) {
	UBRR0L = (uint8_t) baudrate_register_value;
	UBRR0H = (uint8_t) (baudrate_register_value >> 8);
	
	UCSR0B |= (1 << RXCIE0) |  (1 << TXEN0) | (1 << RXEN0);
}

void usart_write_byte(uint8_t data) {
	while (!usart_ready_to_write());
	UDR0 = data;
}

uint8_t usart_read_byte(uint8_t * data) {
	uint16_t timeout_counter = 0;
	
	while (!usart_has_bytes()) {
		uint16_t USART_RECEIVE_TIMEOUT_COUNT = 30000; // Try 60000!!
		if (timeout_counter++ >= USART_RECEIVE_TIMEOUT_COUNT)
			return 1;
	}
	
	*data = usart_receive_buffer[usart_buffer_read_index++];
	return 0;
}

uint8_t usart_has_bytes() {
	return usart_buffer_read_index != usart_buffer_write_index;
}

uint8_t usart_ready_to_write() {
	return UCSR0A & (1 << UDRE0);
}

uint8_t usart_tx_complete() {
	return UCSR0A & (1 << TXC0);
}
