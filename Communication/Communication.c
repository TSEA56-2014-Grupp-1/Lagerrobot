/*
 * Communication.c
 *
 * Created: 2014-03-26 09:49:31
 *  Author: Karl
 */ 
#define ROTATE_INTERVAL 100

#include <avr/io.h>
#include <string.h>
#include "LCD.h"
#include "../shared/bus.h"
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t lcd_rotation_counter = 0;
uint8_t lcd_current_sender = 0;

char message_map_line1[4][16] = {
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

void symbols_are_ready(uint8_t id, uint16_t data) {
	uint16_t symbol_pair;
	switch (data) {
		case 's':
			for (int i = 0; i < 8; ++i) {
				//loop over the symbol pairs
				bus_request(0b0000011, 1, i, &symbol_pair);
				message_map_line1[SENS][2*i] = symbol_pair >> 8;
				message_map_line1[SENS][2*i+1] = symbol_pair;
				
				bus_request(0b0000011, 1, i | 0b00010000, &symbol_pair);
				message_map_line2[SENS][2*i] = symbol_pair >> 8;
				message_map_line2[SENS][2*i+1] = symbol_pair;
				
			}
		break;
		case 'c':
		
		break;
		case 'a':
		
		break;
	}
	
}

void clear_message(uint8_t unit) {
	for (int i = 0; i<16; ++i){
		if (i < 13)
			message_map_line1[unit][i] = 0x20;
		message_map_line2[unit][i] = 0x20;
	}
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
	
	bus_register_response(2, symbols_are_ready);
	_delay_ms(100);
	
	display(COMM, "Hello!", "World!");	
	
	uint16_t sysdata = ((uint16_t) 'H' << 8) | (uint16_t) 'A';
	int8_t result;
	result = bus_request(0b0000011, 1, 0x0123, &sysdata);
	
	clear_message(SENS);
	
	message_map_line1[SENS][0] = (uint8_t) (sysdata >> 8);
	message_map_line1[SENS][1] = (uint8_t) sysdata;
	
	PORTD = result;
    while(1)
    {
		
        //TODO:: Please write your application code 
    }
}