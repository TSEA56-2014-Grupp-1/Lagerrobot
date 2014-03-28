/*
 * Buss.c
 *
 * Created: 3/25/2014 9:58:40 AM
 *  Author: andru411
 */


#include <avr/io.h>
#include <avr/interrupt.h>

#include "bus.h"

uint16_t test(uint8_t id, uint16_t data) {
	return data;
}

int main(void)
{
	DDRA = 0xff;
	DDRB = 0xff;
	PORTA = 0x00;
	PORTB = 0xff;

	bus_init(3);
	bus_register_response(1, test);

	uint16_t i;
	for (i = 0; i < 60000; i++);

	uint16_t data;
	bus_request(1, 1, 0xfff, &data);
	//for(i = 0; i < 100; i++);
	//PORTB = bus_transmit(1, 0xf0, 0x0f);
	bus_transmit(1, 0xf0, 0x0f);
	//PORTA = (uint8_t)data;

	while (1) {
		//PORTB = TWCR;
	}
}
