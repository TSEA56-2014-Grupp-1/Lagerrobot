/*
 * Communication.c
 *
 * Created: 2014-03-26 09:49:31
 *  Author: Karl
 */ 
#define ROTATE_INTERVAL 10

#include <avr/io.h>
#include "LCD.h"
#include "bus.h"
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t lcd_rotation_counter = 0;
uint8_t lcd_current_sender = 0;

char message_map_line1[4][13] = {
	"Kommunikation",
	"Sensor",
	"Arm",
	"Chassi"};
	
char message_map_line2[4][16] = {
	"Kommunicerar",
	"Censurerar",
	"Armerar",
	"Charkuterierar"};

ISR(TIMER1_OVF_vect) {
	if(lcd_rotation_counter == ROTATE_INTERVAL) {
		lcd_rotation_counter = 0;
		
		display(lcd_current_sender,
				message_map_line1[lcd_current_sender],
				message_map_line2[lcd_current_sender]);
		if (lcd_current_sender == 3)
			lcd_current_sender = 0;
		else
			++lcd_current_sender;
	}
	else
		++lcd_rotation_counter;
}

void init(){
	DDRA = 0xff;
	DDRB = 0xff;
	PORTB = 0b00000000;
	PORTA = 0x0;
	
	TIMSK1 = (1 << TOIE1);
	TCCR1A = 0x00; // normal mode
	TCCR1B = 0b00000010; // normal mode, max prescaler; 
	
}


int main(void)
{
	init();
	lcd_init();
	bus_init(0b0000101);
	
	
	
	sei();
	display(COMM, "Hello!", "World!");
    while(1)
    {
		
        //TODO:: Please write your application code 
    }
}