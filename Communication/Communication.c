/*
 * Communication.c
 *
 * Created: 2014-03-26 09:49:31
 *  Author: Karl
 */ 

#include "Communication.h"
#include "LCD.h"
#include "bluetooth.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void init(){
	DDRA = 0xff;
	DDRB = 0xff;
	PORTB = 0b00000000;
	PORTA = 0x0;
}

int main(void)
{
	init();
	//_delay_ms(200);
	lcd_init();
	bt_init();
	sei();
	
	lcd_print("Are you done?", "I want to live!");
    while(1)
    {
        //TODO:: Please write your application code 
    }
}