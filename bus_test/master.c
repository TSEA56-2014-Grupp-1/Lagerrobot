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
	DDRB = 0b11111111;
	PORTB = 0x00;

	bus_init(0b11001010);
	uint16_t i;
	for(i = 0; i < 60000; i++);
	for(i = 0; i < 60000; i++);

	uint16_t data;

	PORTB = bus_receive(0b01100101, &data);

	//Enable interrupts
	sei();

	while(1){
		PORTA = (uint8_t)(data >> 8);
		PORTB = (uint8_t)data;
	};
}
