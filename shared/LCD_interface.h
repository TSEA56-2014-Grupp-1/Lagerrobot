#ifndef LCD_INTERFACE_H_
#define LCD_INTERFACE_H_

#include <avr/io.h>
#include <stdarg.h>

char lcd_display_symbols[2][17];

void lcd_interface_init();

void display(uint8_t line_number, const char* str, ...);

uint16_t symbol_request(uint8_t id, uint16_t data);

#endif