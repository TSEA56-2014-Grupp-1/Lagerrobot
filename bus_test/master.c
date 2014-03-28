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
	PORTB = 0x00;

	bus_init(0b11001010);
	uint16_t i;
	for(i = 0; i < 60000; i++);

	uint16_t data;

	bus_request(0b01100101, 1, 0xfff, &data);
	PORTA = (uint8_t)data;

	while(1) {}
}
