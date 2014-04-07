/*
 * LCD.h
 *
 * Created: 2014-03-25 17:29:45
 *  Author: Karl Linderhed
 */ 


#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>

#define display(line_number, string, ...) _display(line_number, string, sizeof((float[]){0, ##__VA_ARGS__}) / sizeof(float) - 1, ##__VA_ARGS__)

void _display(uint8_t line_number, char* str, uint8_t num_vals, ...);

void lcd_send_symbol(char);
void lcd_send_command(char);
int lcd_is_busy();

void force_display_update(uint8_t module);

void lcd_print(char text1[], char text2[]);
void lcd_clear();

void lcd_init();

void lcd_display(uint8_t sender, char text1[], char text2[]);

#endif /* LCD_H_ */