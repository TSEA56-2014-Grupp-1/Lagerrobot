
#include <avr/io.h>
#include "../shared/bus.h"
#include "../shared/LCD_interface.h"

void display_text(char line1[], char line2[]) {
	for (int i = 0; i < 16; ++i){
		if (i < 13)
		lcd_display_symbols[0][i] = line1[i];
		lcd_display_symbols[1][i] = line2[i];
	}
	
	bus_transmit(0b0000101, 2, 's');
	
}


uint16_t symbol_request(uint8_t id, uint16_t data) {
	//pick out correct symbols from data, bit 5 contains line number and the request will return a pair of symbols
	return	((uint16_t) lcd_display_symbols[(data & 0b00010000) >> 4][(data & 0b00001111) << 1] << 8) |
	((uint16_t) lcd_display_symbols[(data & 0b00010000) >> 4][((data & 0b00001111) << 1) + 1]);
	
}