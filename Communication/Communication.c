/**
 *	@file Communication.c
 *	@author Karl Linderhed
 *
 *	Processes commands to and from PC as well as controlling LCD display
 */

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

/**
 *	Rotate display between each unit
 */
ISR(TIMER1_OVF_vect) {
	if(lcd_rotation_counter == ROTATE_INTERVAL) {
		lcd_rotation_counter = 0;
		lcd_rotation_flag = 1;
	}
	else
        ++lcd_rotation_counter;

}

/**
 *	Trigger stop signal to all units when timer 3 wraps around. This is happens
 *	when the computer has been connected once and communication stops.
 */
ISR(TIMER3_OVF_vect) {
	if (has_connection && !stop_sent) {
		bus_transmit(0, 0, 0);
		stop_sent = 1;
	}
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

/**
 *	Receive character for line 1 for chassi page
 */
void lcd_chassi_line1(uint8_t id, uint16_t metadata) {
	lcd_process_symbol(CHAS, 1, metadata);
}

/**
 *	Receive character for line 2 for chassi page
 */
void lcd_chassi_line2(uint8_t id, uint16_t metadata) {
	lcd_process_symbol(CHAS, 2, metadata);
}

/**
 *	Receive character for line 1 for sensor page
 */
void lcd_sensor_line1(uint8_t id, uint16_t metadata) {
	lcd_process_symbol(SENS, 1, metadata);
}

/**
 *	Receive character for line 2 for sensor page
 */
void lcd_sensor_line2(uint8_t id, uint16_t metadata) {
	lcd_process_symbol(SENS, 2, metadata);
}

/**
 *	Receive character for line 1 for arm page
 */
void lcd_arm_line1(uint8_t id, uint16_t metadata) {
	lcd_process_symbol(ARM, 1, metadata);
}

/**
 *	Receive character for line 2 for arm page
 */
void lcd_arm_line2(uint8_t id, uint16_t metadata) {
	lcd_process_symbol(ARM, 2, metadata);
}

/**
 *	Convert character data sent over the bus to LCD data
 *
 *	@param module Module index (COMM, SENS, ARM or CHAS)
 *	@param line_number 0 means first row, 1 means second row
 *	@param metadata Bit 0-6 is ASCII code for symbol, bit 7-10 is character
 *	                position in row
 */
void lcd_process_symbol(uint8_t module, uint8_t line_number, uint16_t metadata) {
	uint8_t position;
	uint8_t symbol;

	position = (uint8_t) (metadata >> 7); // bits 7-10 contains position data
	symbol = (uint8_t) metadata & 0b01111111; // lowest 7 bits contains ascii symbol

	// Check for null character. If null then the message is finished and the rest of the line should be cleared
	// Otherwise simply fill the message map with the character
	if (line_number == 1) {
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
	else if (line_number == 2) {
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
void init() {
	DDRA = 0xff;
	DDRB = 0xff;
	PORTB = 0b00000000;
	PORTA = 0x0;

	TIMSK1 = (1 << TOIE1);
	TCCR1A = 0x00; // normal mode
	TCCR1B = 0b00000010;

	TIMSK3 = (1 << TOIE3);
	TCCR3A = 0x00;
	TCCR3B = 0b00000100;
	has_connection = 0;

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

/**
 *	Forward decisions from chassis to PC
 */
void forward_decision(uint8_t id, uint16_t data) {
	send_packet(PKT_CHASSIS_DECISION, 1, (uint8_t)data);
}

/**
 *	Forward RFID data from sensor unit to PC
 */
void forward_RFID(uint8_t id, uint16_t data) {
	send_packet(PKT_RFID_DATA, 1, data);
}

/**
 *	Forward side scanner data from sensor unit to PC
 */
void forward_range(uint8_t id, uint16_t data) { // MSB indicates which sensor is scanning
	send_packet(PKT_RANGE_DATA, 3, (uint8_t) (data >> 10), (uint8_t) ((data & 0b01100000000) >> 8), (uint8_t) data);
}

/**
 *	Forward line sensor data from sensor unit to PC
 */
void forward_line_data(uint8_t id, uint16_t data) {
	if (id == 11) {
		send_packet(PKT_LINE_WEIGHT, 1, (uint8_t)data);
	} else {
		send_packet(PKT_LINE_DATA, 2, (uint8_t)(data >> 8), (uint8_t)data);
	}
}

/**
 *	Forward calibration data from sensor unit to PC
 */
void forward_calibration(uint8_t id, uint16_t data)	{
	send_packet(PKT_CALIBRATION_DATA, 1,(uint8_t)data);
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

	bus_register_receive(8, forward_decision);
	bus_register_receive(9, forward_RFID);
	bus_register_receive(10, forward_range);
	bus_register_receive(11, forward_line_data);
	bus_register_receive(12, forward_line_data);
	bus_register_receive(13, forward_calibration);


	display(0, "Ouroborobot");
	display(1, "Soon...");

	char current_message_map1[17];
	char current_message_map2[17];
	uint8_t lcd_current_sender;

	for (;;) {
		// Wait for either display to rotate or data from computer
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
			usart_clear_buffer();

		}
	}
}
