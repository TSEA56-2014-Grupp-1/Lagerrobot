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
	bus_init(0b01100101);
	sei();
	while(1);
}
