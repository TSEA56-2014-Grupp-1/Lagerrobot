/*
 * LCD.c
 *
 * Created: 2014-03-25 17:29:08
 *  Author: Karl Linderhed
 */ 

#include "LCD.h"
#include "Communication.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdarg.h>
#include <stdio.h>



/**
 * @brief Initializes the lcd display controller.
 */
void lcd_init() {
	
	//initialize reset in controller
	PORTB &= ~(1 << PORTB0);
	PORTA = 0b00110000;
	PORTB |= 1 << PORTB2;
	_delay_ms(4.1);
	PORTB &= ~(1 << PORTB2);
	//PORTA = 0b00110000;
	PORTB |= 1 << PORTB2;
	_delay_us(100);
	PORTB &= ~(1 << PORTB2);
	//PORTA = 0b00110000;
	PORTB |= 1 << PORTB2;
	_delay_us(100);
	PORTB &= ~(1 << PORTB2);
	
	lcd_send_command(0b00111000); // 8 bit, 2 lines
	lcd_send_command(0b00001000); // display off
	lcd_clear();
	lcd_send_command(0b00000110); // left-to-right, no shift
	
	lcd_send_command(0b00001100); // display on
	
}

/**
 * @brief Sends a single symbol to the display.
 * @details This will send a symbol to the current DDRAM address on the display controller
 * and automatically increment the display cursor.
 * 
 * @param symbol The symbol to be printed.
 */
void lcd_send_symbol(char symbol) {
	while (lcd_is_busy())
		_delay_ms(1);
	
	PORTB |= 1 << 0;
	PORTA = symbol; 
	PORTB |= 1 << 2; 
	_delay_us(50); // lcd controller execution time
	PORTB &= ~(1 << 2); 
	
}

/**
 * @brief Sends a command to the display controller.
 * 
 * @param cmd The command code to be sent.
 */
void lcd_send_command(char cmd) {
	while (lcd_is_busy())
		_delay_ms(1);
		
	PORTB &= ~(1 << 0); 
	PORTA = cmd; 
	PORTB |= 1 << 2; 
	if (cmd == 0b01 || cmd == 0b10) // if clear or return home instruction
		_delay_ms(1.5);				// then execution time is longer
	else
		_delay_us(50);
		
	
	PORTB &= ~(1 << 2);
}

/**
 * @brief Checks if the display is busy.
 * @details Queries the display controller if it is able to receive instructions or data.
 * @return The value of the busy flag. 1 (true) means the display is busy.
 */
int lcd_is_busy(){
	DDRA = 0;
	PORTB |= 1 << 1;
	PORTB &= ~(1 << 0);
	PORTB |= 1 << 2;
	_delay_us(2);
	char data = PINA;
	PORTB &= ~(1 << 2);
	PORTB &= ~(1 << 1);
	DDRA = 0xff;
	return data >> 7; // MSB is the busy flag
}

/**
 * @brief Prints two lines of text on the display.
 * 
 * @param text1 The first line of text.
 * @param text2 The second line of text.
 */
void lcd_print(char text1[16], char text2[16]) {
	lcd_clear();
	for(int i=0; text1[i] != '\0' && i < 16; i++)
		lcd_send_symbol(text1[i]);
		
	lcd_send_command(0b11000000); // line 2, DDRAM address 64
	for(int i=0; text2[i] != '\0' && i < 16; i++)
		lcd_send_symbol(text2[i]);
}

/**
 * @brief Clears the display.
 */
void lcd_clear() {
	lcd_send_command(1);
}

/**
 * @brief Formats a page to a particular sender and displays it.
 * @details Prepends a letter to the first line of text and displays it on the display. 
 * The letter indicates to which unit the current page belongs. Note that the first line 
 * of text can only be 13 characters long as a result.
 * @param sender The identifier of th unit that sent the text.
 * @param text1 The first line of text.
 * @param text2 The second line of text.
 */
void lcd_display(uint8_t sender, char text1[13], char text2[16]){
	char line1[16];
	char line2[16];
	switch (sender) {
		case COMM:
			line1[0] = 'K';
		break;
		case SENS:
			line1[0] = 'S';
		break;
		case ARM:
			line1[0] = 'A';
		break;
		case CHAS:
			line1[0] = 'C';
		break;
	}
	
	line1[1] = ':';
	line1[2] = ' ';
	
	for (int i = 0; i < 16; ++i) {
		if (i > 2)
			line1[i] = text1[i-3];
		line2[i] = text2[i];
	}
	
	
	lcd_print(line1, line2);
}

/**
 * @brief Displays a formatted string of text and variables on the display.
 * @details This is a special function for the Communication unit, since it can't
 * use the LCD_interface to send text across the bus. It encapsulates a standard printf
 * function that can take any number of variables as parameters.
 * 
 * @param line_number The line on which text is to be printed. 0 is the first line, 1 is the second.
 * @param str The format string to display.
 * @param ... The variables to display.
 */
void display(uint8_t line_number, char* str, ...) {
	va_list data;
	
	va_start(data, str);
	if (line_number == 0)
	vsnprintf(message_map_line1[COMM], 17, str, data);
	else if (line_number == 1)
	vsnprintf(message_map_line2[COMM], 17, str, data);
	va_end(data);
	
	lcd_force_display_update(COMM);
}