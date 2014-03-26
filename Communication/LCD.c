/*
 * LCD.c
 *
 * Created: 2014-03-25 17:29:08
 *  Author: Karl Linderhed
 */ 
#define F_CPU 18.432E6

#include <util/delay.h>
#include <avr/io.h>
#include "LCD.h"



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



void lcd_send_symbol(char symbol) {
	while (lcd_is_busy())
		_delay_ms(1);
	
	PORTB |= 1 << 0;
	PORTA = symbol; 
	PORTB |= 1 << 2; 
	_delay_us(50); // lcd controller execution time
	PORTB &= ~(1 << 2); 
	
}

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

void lcd_print(char text1[], char text2[]) {
	if (sizeof(text1) > 17)
		text1[12] = "#ERR";
	
	if (sizeof(text2) > 17)
		text2[12) = "#ERR"
		
	lcd_clear();
	for(int i=0; text1[i] != '\0' && i < 17; i++)
		lcd_send_symbol(text1[i]);
		
	lcd_send_command(0b11000000); // line 2
	for(int i=0; text2[i] != '\0' && i < 17; i++)
	lcd_send_symbol(text2[i]);
}

void lcd_clear() {
	lcd_send_command(1);
}