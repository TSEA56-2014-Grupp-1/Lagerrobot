/*
 * Communication.c
 *
 * Created: 2014-03-26 09:49:31
 *  Author: Karl
 */ 
#define ROTATE_INTERVAL 100

#include "Communication.h"
#include "LCD.h"
#include "../shared/bus.h"
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t lcd_rotation_counter = 0;
uint8_t lcd_current_sender = 0;

char message_map_line1[4][16] = {
	"",
	"",
	"",
	""};
	
char message_map_line2[4][16] = {
	"",
	"",
	"",
	""};

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
	uint8_t module;
	switch(data) {
		case BUS_ADDRESS_ARM:
		module = ARM;
		break;
		case BUS_ADDRESS_CHASSIS:
		module = CHAS;
		break;
		case BUS_ADDRESS_SENSOR:
		module = SENS;
		break;
		default:
		module = 0;
	}
	
	for (int i = 0; i < 8; ++i) {
		//loop over the symbol pairs
		bus_request(data, 1, i, &symbol_pair);
		message_map_line1[module][2*i] = symbol_pair >> 8;
		message_map_line1[module][2*i+1] = symbol_pair;
		
		//fifth bit contains line data
		bus_request(data, 1, i | 0b00010000, &symbol_pair);
		message_map_line2[module][2*i] = symbol_pair >> 8;
		message_map_line2[module][2*i+1] = symbol_pair;
				
	}
	
	// jump to the module that sent last
	lcd_current_sender = module;
	TCNT1 = 0xffff;
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
	
	bus_register_receive(2, symbols_are_ready);
	
	display(COMM, "Hello!", "World!");	
	
    while(1)
    {
		
        //TODO:: Please write your application code 
    }
}