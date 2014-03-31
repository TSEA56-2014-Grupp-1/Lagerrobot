/*
 * lcd_test.c
 *
 * Created: 3/28/2014 4:34:09 PM
 *  Author: karli315
 */ 


#include <avr/io.h>
#include "../shared/bus.h"

#include <avr/interrupt.h>

uint8_t lcd_display_symbols[2][16];

uint16_t symbol_request(uint8_t id, uint16_t data) {
	//pick out correct symbols from data, bit 5 contains line number and the request will return a pair of symbols
	return	((uint16_t) lcd_display_symbols[(data & 0b00010000) >> 4][(data & 0b00001111) << 1] << 8) |
			((uint16_t) lcd_display_symbols[(data & 0b00010000) >> 4][((data & 0b00001111) << 1) + 1] << 8);
	
}

void display_text(uint8_t line1[16], uint8_t line2[16]) {
	lcd_display_symbols[0] = line1;
	lcd_display_symbols[1] = line2;
	bus_transmit(0b0000101, 2, (uint16_t) 's');
}

int main(void)
{
	bus_init(0b0000011);
	
	bus_register_response(1, symbol_request);
	
	
	sei();
	_delay_ms(200);
	display_text("Testar", "Bus-display");
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}