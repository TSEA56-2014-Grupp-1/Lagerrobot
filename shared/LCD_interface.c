#include "../shared/LCD_interface.h"
#include "../shared/bus.h"
#include <avr/io.h>
#include <stdio.h>
#include <stdarg.h>

void lcd_interface_init(){
	bus_register_response(1, symbol_request);
}

void display_clear() {
	for (int i = 0; i < 16; ++i) {
		lcd_display_symbols[0][i] = ' ';
		lcd_display_symbols[1][i] = ' ';
	}
}

void display(uint8_t line_number, const char* str, ...) {
	
	va_list data;
	va_start(data, str);
	vsnprintf(lcd_display_symbols[line_number], 17, str, data);
	va_end(data);
	
	bus_transmit(BUS_ADDRESS_COMMUNICATION, 2, bus_get_address());
}


uint16_t symbol_request(uint8_t id, uint16_t data) {
	//pick out correct symbols from data, bit 5 contains line number and the request will return a pair of symbols
	return	((uint16_t) lcd_display_symbols[(data & 0b00001000) >> 3][(data << 1) & 0b00001111] << 8) |
			((uint16_t) lcd_display_symbols[(data & 0b00001000) >> 3][((data << 1) & 0b00001111) + 1]);
}