/*
 * LCD.h
 *
 * Created: 2014-03-25 17:29:45
 *  Author: Karl Linderhed
 */ 


#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>

void lcd_send_symbol(char);
void lcd_send_command(char);
int lcd_is_busy();

void lcd_print(char text1[], char text2[]);
void lcd_clear();

void lcd_init();

void display(uint8_t sender, char text1[], char text2[]);

#endif /* LCD_H_ */