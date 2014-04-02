/*
 * bluetooth.c
 *
 * Created: 2014-04-01 15:35:48
 *  Author: Karl
 */ 

#include "bluetooth.h"
#include "Communication.h"
#include "../shared/packets.h"
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(USART0_RX_vect) {
	UCSR0B &= ~(1 << RXCIE0); // disable further interrupts
	
	uint8_t packet_params[UDR0];
	
	bt_wait_rx_done();
	uint8_t packet_id = UDR0;
	
	for (int i = 0; i < sizeof(packet_params); ++i) {
		bt_wait_rx_done();
		packet_params[i] = UDR0;
	}
	
	bt_process_packet(packet_id, packet_params);
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

void bt_process_packet(uint8_t packet_id, uint8_t parameters[]) {
	switch (packet_id) {
		case PKT_STOP:
			
		break;
		case PKT_ARM_COMMAND:
			switch (parameters[0]) {
				case CMD_ARM_MOVE:
					
				break;
				case CMD_ARM_GRIP:
				
				break;
				case CMD_ARM_RELEASE:
				
				break;
				case CMD_ARM_PREDEFINED_POS:
				
				break;
			}
		break;
		case PKT_CHASSIS_COMMAND:
			switch (parameters[0]) {
				case CMD_CHASSIS_SPEED:
				
				break;
				case CMD_CHASSIS_STEER:
				
				break;
				case CMD_CHASSIS_START:
				
				break;
			}
		break;
		case PKT_CALIBRATION_COMMAND:
		
		break;
		case PKT_PACKET_REQUEST:
		
		break;
		case PKT_SPOOFED_REQUEST:
		
		break;
		case PKT_SPOOFED_TRANSMIT:
		
		break;
	}
}

void bt_wait_rx_done() {
	while (!(UCSR0A & (1 << RXC0)));
}

void bt_wait_ready_to_tx() {
	while (!(UCSR0A & (1 << UDRE0)));
}

void bt_wait_tx_done() {
	while (!(UCSR0A & (1 << TXC0)));
} 
