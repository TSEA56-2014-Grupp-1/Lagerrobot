/*
 * Buss.c
 *
 * Created: 3/25/2014 9:58:40 AM
 *  Author: andru411
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "bus.h"

uint8_t runtrans = 0;

uint16_t test(uint8_t id, uint16_t data) {
	runtrans = 1;
	return data;
}

int main(void)
{
	// Master testing program
	DDRA = 0xff;
	DDRB = 0xff;
	PORTA = 0x00;
	PORTB = 0x00;

	bus_init(1);
	bus_register_response(1, test);
	uint16_t i;
	//for(i = 0; i < 60000; i++);

	uint16_t data;

	//bus_request(0b01100101, 1, 0xfff, &data);
	//PORTA = (uint8_t)data;

	PORTB = 0xff;

	while(1) {
		if (runtrans) {
			PORTB = bus_transmit(3, 0xf0, 0x0f);
			runtrans = 0;
		}
	}
}
