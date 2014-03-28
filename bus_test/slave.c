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
	bus_init(0b01100101);
	bus_register_response(1, test);
	while(1);
}
