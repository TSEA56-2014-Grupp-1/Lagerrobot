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
 *	lcd_init, initializes the lcd interface
 */
void lcd_init() {
	
	//initialize reset in controller
	PORTB &= ~(1 << 0);
	PORTA = 0b00110000;
	PORTB |= 1 << 2;
	_delay_ms(4.1);
	PORTB &= ~(1 << 2);
	//PORTA = 0b00110000;
	PORTB |= 1 << 2;
	_delay_us(100);
	PORTB &= ~(1 << 2);
	//PORTA = 0b00110000;
	PORTB |= 1 << 2;
	_delay_us(100);
	PORTB &= ~(1 << 2);
	
	lcd_send_command(0b00111000); // 8 bit, 2 lines
	lcd_send_command(0b00001000); // display off
	lcd_clear();
	lcd_send_command(0b00000110); // left-to-right, no shift
	
	lcd_send_command(0b00001100); // display on
	
}


/**
 *	lcd_send_symbol, sends a symbol to the display.
 *	
 *	@param symbol The ASCII symbol to be sent
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
 *	lcd_send_command, sends an instruction to the display controller.
 *
 *	@param cmd The command code to be sent
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
 *	lcd_is_busy, checks if the display controller can accept data or instructions.
 * 
 *	@return A boolean value that is true if the lcd is unable to accept data or instructions.
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
 *	lcd_print, prints two lines of text on the display
 *
 *	@param text1, the first line of text to be printed
 *	@param text2, the second line of text to be printed
 */
void lcd_print(char text1[], char text2[]) {
	lcd_clear();
	for(int i=0; text1[i] != '\0' && i < 17; i++)
		lcd_send_symbol(text1[i]);
		
	lcd_send_command(0b11000000); // line 2, DDRAM address 64
	for(int i=0; text2[i] != '\0' && i < 17; i++)
	lcd_send_symbol(text2[i]);
}

/**
 *	lcd_clear, clears the display
 */
void lcd_clear() {
	lcd_send_command(1);
}

void lcd_display(uint8_t sender, char text1[13], char text2[16]){
	char line1[16];
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
	line1[3] = text1[0];
	line1[4] = text1[1];
	line1[5] = text1[2];
	line1[6] = text1[3];
	line1[7] = text1[4];
	line1[8] = text1[5];
	line1[9] = text1[6];
	line1[10] = text1[7];
	line1[11] = text1[8];
	line1[12] = text1[9];
	line1[13] = text1[10];
	line1[14] = text1[11];
	line1[15] = text1[12];
	
	lcd_print(line1, text2);
}

void _display(uint8_t line_number, char* str, uint8_t num_vals, ...) {
	va_list data;
	
	va_start(data, num_vals);
	if (line_number == 0)
		vsnprintf(message_map_line1[COMM], 17, str, data);
	else if (line_number == 1)
		vsnprintf(message_map_line2[COMM], 17, str, data);  
	va_end(data);
	
	lcd_current_sender = COMM;
	TCNT1 = 0xfff0;
}