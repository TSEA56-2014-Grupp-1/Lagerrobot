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

void test2(uint8_t id, uint16_t data) {
	PORTB = 0xff;
}

int main(void)
{
	DDRA = 0xff;
	DDRB = 0xff;
	PORTA = 0x00;
	PORTB = 0x00;

	bus_init(3);
	bus_register_response(1, test);
	bus_register_receive(2, test2);

	while (1) {
		//PORTB = TWCR;
	}
}
