#ifndef LCD_H_
#define LCD_H_
/**
 *	@file LCD.h
 *	@author Karl Linderhed
 *
 *	Functions for controlling LCD display
 */

#include <avr/io.h>

void display(uint8_t line_number, char* str, ...);
void lcd_send_symbol(char);
void lcd_send_command(char);
void force_display_update(uint8_t module);
int lcd_is_busy();
void lcd_print(char text1[], char text2[]);
void lcd_clear();
void lcd_init();
void lcd_display(uint8_t sender, char text1[], char text2[]);

#endif /* LCD_H_ */
