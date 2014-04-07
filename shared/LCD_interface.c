#include "../shared/LCD_interface.h"
#include "../shared/bus.h"
#include <avr/io.h>
#include <stdio.h>
#include <stdarg.h>

/**
 * @brief Registers the symbol request with the bus.
 * @details Masks the bus plumbing from the lcd user.
 */
void lcd_interface_init(){
	bus_register_response(1, symbol_request);
}

/**
 * @brief Clears the display page of the current unit.
 * @details Sets all display positions to space (' ').
 */
void display_clear() {
	for (int i = 0; i < 16; ++i) {
		lcd_display_symbols[0][i] = ' ';
		lcd_display_symbols[1][i] = ' ';
	}
}

/**
 * @brief Displays a formatted string of text and variables on the display.
 * @details Uses variable arguments to encapsulate a standard printf, which takes 
 * a formatted string and any number of variables.
 * 
 * @param line_number The line on which text is to be printed. 0 is the first line, 1 is the second.
 * @param str The format string to display.
 * @param ... The variables to display.
 */
void display(uint8_t line_number, const char* str, ...) {
	
	va_list data;
	va_start(data, str);
	vsnprintf(lcd_display_symbols[line_number], 17, str, data);
	va_end(data);
	
	bus_transmit(BUS_ADDRESS_COMMUNICATION, 2, bus_get_address());
}

/**
 * @brief Responds to a request för display symbols.
 * @details A standard bus callback for a request from the communication module for symbols to 
 * print on the display. The request is made with a symbol pair index as metadata, and returns 
 * two symbols. Example, the index 0 will return the first and second symbol, index 2 the fifth and sixth.
 * 
 * @param id Standard callback parameter, the id of the request.
 * @param data Standard callback parameter, here it is the index of the symbol pair requested.
 * 
 * @return [description]
 */
uint16_t symbol_request(uint8_t id, uint16_t data) {
	//pick out correct symbols from data, bit 5 contains line number and the request will return a pair of symbols
	return	((uint16_t) lcd_display_symbols[(data & 0b00001000) >> 3][(data << 1) & 0b00001111] << 8) |
			((uint16_t) lcd_display_symbols[(data & 0b00001000) >> 3][((data << 1) & 0b00001111) + 1]);
}