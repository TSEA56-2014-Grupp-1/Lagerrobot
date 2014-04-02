/*
 * bluetooth.c
 *
 * Created: 2014-04-01 15:35:48
 *  Author: Karl
 */ 

#include "bluetooth.h"
#include "Communication.h"
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(USART0_RX_vect) {
	int8_t data;
	
	data = UDR0;
	bt_wait_rx_done();
	bt_send_byte(data);
}

void bt_init(){
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (3 << UCSZ00);
	UBRR0L = 0x09;
	UBRR0H = 0x00;
	
}

void bt_send_byte(uint8_t data) {
	UDR0 = data;
	bt_wait_tx_done();
}

void bt_wait_rx_done() {
	while (!(UCSR0A & (1 << UDRE0)));
}

void bt_wait_tx_done() {
	while (!(UCSR0A & (1 << TXC0)));
} 