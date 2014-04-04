#ifndef LCD_INTERFACE_H_
#define LCD_INTERFACE_H_

#include <avr/io.h>
#include <stdarg.h>
#define display(line_number, string, ...) _display(line_number, string, sizeof((float[]){0, ##__VA_ARGS__}) / sizeof(float) - 1, ##__VA_ARGS__)

char lcd_display_symbols[2][17];

void lcd_interface_init();

void _display(uint8_t line_number, const char* str, uint8_t num_vals, ...);

uint16_t symbol_request(uint8_t id, uint16_t data);

#endif