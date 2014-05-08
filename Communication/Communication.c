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
#include "pc_link.h"
#include "../shared/packets.h"
#include "../shared/usart.h"
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(TIMER1_OVF_vect) {
	if(lcd_rotation_counter == ROTATE_INTERVAL) {
		lcd_rotation_counter = 0;
		lcd_rotation_flag = 1;
	}
	else
        ++lcd_rotation_counter;
	
}

/**
 * @brief Forces the display to display the page of a certain module.
 * @details Resets the rotation counter and outputs the page of a certain module to the display.
 *
 * @param module The identifier of the module to be displayed.
 */
void lcd_force_display_update(uint8_t module) {
	lcd_rotation_counter = ROTATE_INTERVAL;
	lcd_next_sender = module;
}

void lcd_chassi_line1(uint8_t id, uint16_t metadata) {
	lcd_process_symbol(CHAS, 1, metadata);
}

void lcd_chassi_line2(uint8_t id, uint16_t metadata) {
	lcd_process_symbol(CHAS, 2, metadata);
}

void lcd_sensor_line1(uint8_t id, uint16_t metadata) {
	lcd_process_symbol(SENS, 1, metadata);
}

void lcd_sensor_line2(uint8_t id, uint16_t metadata) {
	lcd_process_symbol(SENS, 2, metadata);
}

void lcd_arm_line1(uint8_t id, uint16_t metadata) {
	lcd_process_symbol(ARM, 1, metadata);
}

void lcd_arm_line2(uint8_t id, uint16_t metadata) {
	lcd_process_symbol(ARM, 2, metadata);
}

void lcd_process_symbol(uint8_t module, uint8_t line_number, uint16_t metadata) {
	uint8_t position;
	uint8_t symbol;
	
	position = (uint8_t) (metadata >> 7); // bits 7-10 contains position data
	symbol = (uint8_t) metadata & 0b01111111; // lowest 7 bits contains ascii symbol
	
	// Check for null character. If null then the message is finished and the rest of the line should be cleared
	// Otherwise simply fill the message map with the character
	if (line_number == 1){
		if (symbol == 0x00) {
			for (uint8_t i =  position; i < 16; ++i) {
				message_map_line1[module][i] = 0x20;
			}
			lcd_force_display_update(module);
		}
		else {
			message_map_line1[module][position] = symbol;
		}
	}
	else if (line_number == 2){
		if (symbol == 0x00) {
			for (uint8_t i =  position; i < 16; ++i) {
				message_map_line2[module][i] = 0x20;
			}
			lcd_force_display_update(module);
		}
		else {
			message_map_line2[module][position] = symbol;
		}
	}
	
}


/**
 * @brief Clears the display page of a unit.
 * @details Clears the stored display page of a unit, but does not update the display.
 *
 * @param unit The identifier of the module whose page is to be cleared.
 */
void clear_message(uint8_t unit, uint8_t line_number) {
	if (line_number == 0) {
		for (int i = 0; i<16; ++i){
			message_map_line1[unit][i] = 0x20;
		}
		message_map_line1[unit][16] = '\0';
	}
	else if (line_number == 1) {
		for (int i = 0; i<16; ++i){
			message_map_line2[unit][i] = 0x20;
		}
		message_map_line2[unit][16] = '\0';
	}
	
	
}


/**
 * @brief Initializes the communication unit.
 * @details Sets up the ports for the display communication, the timers for
 * page rotation and clears the lcd variables and messages.
 */
void init(){
	DDRA = 0xff;
	DDRB = 0xff;
	PORTB = 0b00000000;
	PORTA = 0x0;
	
	TIMSK1 = (1 << TOIE1);
	TCCR1A = 0x00; // normal mode
	TCCR1B = 0b00000010; // normal mode, max prescaler;
	
	lcd_next_sender = 0;
	lcd_rotation_counter = 0;
	lcd_rotation_flag = 0;
	clear_message(COMM, 0);
	clear_message(COMM, 1);
	clear_message(SENS, 0);
	clear_message(SENS, 1);
	clear_message(CHAS, 0);
	clear_message(CHAS, 1);
	clear_message(ARM, 0);
	clear_message(ARM, 1);
}


int main(void)
{
	init();
	lcd_init();
	usart_init(0x0009);
	bus_init(BUS_ADDRESS_COMMUNICATION);
	
	
	bus_register_receive(2, lcd_sensor_line1);
	bus_register_receive(3, lcd_sensor_line2);
	bus_register_receive(4, lcd_arm_line1);
	bus_register_receive(5, lcd_arm_line2);
	bus_register_receive(6, lcd_chassi_line1);
	bus_register_receive(7, lcd_chassi_line2);
	
	
	
	display(0, "Ouroborobot");
	display(1, "Soon...");
	
	char current_message_map1[17];
	char current_message_map2[17];
	uint8_t lcd_current_sender;
	
	while(1)
	{
		while(!lcd_rotation_flag && !usart_has_bytes());

		if (lcd_rotation_flag) {
			cli();
			lcd_rotation_flag = 0;
			lcd_current_sender = lcd_next_sender;
			memcpy(current_message_map1, message_map_line1[lcd_current_sender], 17);
			memcpy(current_message_map2, message_map_line2[lcd_current_sender], 17);
			sei();
			
			lcd_display(lcd_current_sender,
			current_message_map1,
			current_message_map2);
			
			if (!lcd_rotation_flag)
			lcd_next_sender = (lcd_next_sender + 1) % 4;
		}
	
		else {
			process_packet();
			usart_reset_buffer();

		}
	}
}