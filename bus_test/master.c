/*
 * Buss.c
 *
 * Created: 3/25/2014 9:58:40 AM
 *  Author: andru411
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "bus.h"

int main(void)
{
	// Master testing program
	DDRA = 0xff;
	DDRB = 0xff;
	PORTA = 0x00;
	PORTB = 0x00;

	bus_init(1);

	uint16_t i;
	for (i = 0; i < 60000; i++);

	uint16_t data;
	PORTA = bus_request(3, 1, 0xffff, &data);
	PORTB = (uint8_t)data;
	bus_transmit(3, 2, 0xffff);

	while(1) {}
}
