/*
 * Communication.c
 *
 * Created: 2014-03-26 09:49:31
 *  Author: Karl
 */ 

#include "Communication.h"
#include "LCD.h"
#include "pc_link.h"
#include "../shared/usart.h"
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
	usart_init(0x0009);
	sei();
	
	lcd_print("Testing BT", "    ...    ");
	
	
    while(1)
    {
		while (!usart_has_bytes());
		if (process_packet() == 1) // timeout
			break;
		
		
    }
}