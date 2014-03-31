/*
 * lcd_test.c
 *
 * Created: 3/28/2014 4:34:09 PM
 *  Author: karli315
 */ 


#include <avr/io.h>
#include "../shared/bus.h"

#include <avr/interrupt.h>

uint16_t symbol_request(uint8_t id, uint16_t data) {
	uint16_t result;
	result = data + 1;
	return result;
}

int main(void)
{
	bus_init(0b0000011);
	
	bus_register_response(1, symbol_request);
	sei();
    while(1)
    {
        //TODO:: Please write your application code 
    }
}