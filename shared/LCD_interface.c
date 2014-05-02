#include "../shared/LCD_interface.h"
#include "../shared/bus.h"
#include <avr/io.h>



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
	char display_symbols[17];
	
	uint8_t status;
	
	uint8_t module_identifier = 0;
	
	for (uint8_t i = 0; i < 16; ++i){
		display_symbols[i] = 0x20;
	}
	
	va_list data;
	va_start(data, str);
	vsnprintf(display_symbols, 17, str, data);
	va_end(data);
	
	switch (bus_get_address()) {
		case BUS_ADDRESS_SENSOR:
			module_identifier = 1;
		break;
		case BUS_ADDRESS_ARM:
			module_identifier = 2;
		break;
		case BUS_ADDRESS_CHASSIS:
			module_identifier = 3;
		break;
	}
	
	if (module_identifier == 0) // bus has not been initialized correctly!
		return;
		
	for (uint8_t i = 0; i < 16; ++i){
		
		do {
			status = bus_transmit(BUS_ADDRESS_COMMUNICATION, 2*module_identifier + line_number, ((uint16_t) i << 7) | (display_symbols[i] & 0b01111111));
			
		} while (status != 0);
			
		if (display_symbols[i] == 0x00) 
			break;
	}
}
