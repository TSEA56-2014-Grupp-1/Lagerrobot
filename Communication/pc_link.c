/*
* bluetooth.c
*
* Created: 2014-04-01 15:35:48
*  Author: Karl
*/

#include "pc_link.h"
#include "Communication.h"
#include "../shared/bus.h"
#include "../shared/packets.h"
#include "../shared/usart.h"
#include "LCD.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdarg.h>

uint8_t process_packet() {
	uint8_t packet_id;
	
	if (usart_read_byte(&packet_id) == 1)
	return 1;
	
	// 	//if (usart_read_byte(&num_parameters) == 1)
	// 		return 1;
	
	if (packet_id == PKT_STOP) {
		// issue stop command here
	}
	else if (packet_id == PKT_ARM_COMMAND) {
		uint8_t command;
		
		display(0, "arm command!");
		
		if (usart_read_byte(&command) == 1)
			return 1;
			
		if (command == CMD_ARM_MOVE) {
			display(0, "arm move!");
			uint8_t joint;
			uint8_t pos_l;
			uint8_t pos_h;
			
			if (usart_read_byte(&joint) == 1 ||
			usart_read_byte(&pos_l) == 1 ||
			usart_read_byte(&pos_h) == 1)
			return 1;
			// issue arm move command on bus here
			display(1, "j%x pos%x", joint, ((uint16_t) pos_h << 8) | (uint16_t) pos_l);
			//send_packet(PKT_ARM_STATUS, 3, joint, pos_l, pos_h);
			usart_write_byte(PKT_ARM_STATUS);
			usart_write_byte(3);
			usart_write_byte(joint);
			usart_write_byte(pos_h);
			usart_write_byte(pos_l);
		}
		else if (command == CMD_ARM_GRIP) {
			// issue arm grip command on bus here
			
		}
		else if (command ==CMD_ARM_RELEASE) {
			// issue arm release command on bus here
		}
		else if (command ==CMD_ARM_PREDEFINED_POS) {
			uint8_t predefine_id;
			if (usart_read_byte(&predefine_id) == 1)
			return 1;
			
			// issue predefine position command on bus here
			
		}
	}
	else if (packet_id ==PKT_CHASSIS_COMMAND) {
		uint8_t command;
		
		if (usart_read_byte(&command) == 1)
		return 1;
		
		if (command == CMD_CHASSIS_SPEED) {
			
			uint8_t speed;		
			if (usart_read_byte(&speed) == 1)
			return 1;
			
			// issue chassis set speed command on bus here
		}
		else if (command ==CMD_CHASSIS_STEER ) {
					
			uint8_t steering_power;
			if (usart_read_byte(&steering_power) == 1)
			return 1;
			
			// issue chassis set steering power command on bus here
		}
		else if (command ==CMD_CHASSIS_START) {
			// issue chassis start line following / competition on bus here
			
		}
	}
	else if (packet_id ==PKT_CALIBRATION_COMMAND) {
		uint8_t command;
		display(0, "Got calibrate");
		if (usart_read_byte(&command) == 1)
		return 1;
		if (command == CAL_LINE){
			uint8_t is_on_tape;
			display(1, "Line...");
			if (usart_read_byte(&is_on_tape) == 1)
				return 1;
			display(1, "Line, tape=%d", is_on_tape);
			bus_transmit(BUS_ADDRESS_SENSOR, 2, is_on_tape);
		}
		else if (command ==CAL_RANGE) {
			uint8_t distance;
			if (usart_read_byte(&distance) == 1)
				return 1;

			// issue range sensor calibration command on bus here
		}
	}

	else if (packet_id == PKT_PACKET_REQUEST) {
		uint8_t req_packet_id;
		
		display(0, "Got pkt req");
		if (usart_read_byte(&req_packet_id) == 1) 
			return 1;
		
		if (req_packet_id == PKT_LINE_DATA) {
			uint8_t line_values[11];
			uint8_t flags;
			uint8_t center_mass;
			
			display(1, "Line data");
			
			uint16_t temp_line_value_pair;
			for (int i = 0; i < 6; ++i) {
				bus_request(BUS_ADDRESS_SENSOR, 3, i, &temp_line_value_pair);
				line_values[2*i] = (uint8_t) temp_line_value_pair;
				line_values[2*i+1] = (uint8_t) (temp_line_value_pair >> 8);
			}
			
			uint16_t temp_flags_and_center;
			
			bus_request(BUS_ADDRESS_SENSOR, 4, 0, &temp_flags_and_center);
			center_mass = (uint8_t) temp_flags_and_center;
			flags = (uint8_t) (temp_flags_and_center >> 8);
			
			send_packet(PKT_LINE_DATA, 
						13, 
						line_values[0],
						line_values[1],
						line_values[2],
						line_values[3],
						line_values[4],
						line_values[5],
						line_values[6],
						line_values[7],
						line_values[8],
						line_values[9],
						line_values[10],
						flags,
						center_mass);
		}
	}
	else if (packet_id ==PKT_SPOOFED_REQUEST) {
		
		uint8_t address;
		uint8_t request_id;
		uint8_t metadata_h;
		uint8_t metadata_l;
		
		uint16_t response;
				
		if (usart_read_byte(&address) == 1	||
		usart_read_byte(&request_id) == 1	||
		usart_read_byte(&metadata_h) == 1	||
		usart_read_byte(&metadata_l) == 1)
		return 1;
		display(0, "req %d", request_id);
		bus_request(address,
		request_id,
		((uint16_t)metadata_h << 8) | (uint16_t) metadata_l,
		&response);
		
		
		send_packet(PKT_SPOOFED_RESPONSE, 2, (uint8_t)(response >> 8), (uint8_t) response);
		
		display(1, "%x", response);
		
	}
	else if (packet_id ==PKT_SPOOFED_TRANSMIT) {
		uint8_t address;
		uint8_t transmission_id;
		uint8_t metadata_h;
		uint8_t metadata_l;
		
		if (usart_read_byte(&address) == 1	||
		usart_read_byte(&transmission_id) == 1	||
		usart_read_byte(&metadata_h) == 1	||
		usart_read_byte(&metadata_l) == 1)
		return 1;
		
		bus_transmit(address,
		transmission_id,
		((uint16_t)metadata_h << 8) | (uint16_t) metadata_l);
		
		
	}
	return 0;
}


void send_packet(uint8_t packet_id, uint8_t num_parameters, ...) { // uint8_t parameters[]) {
	va_list parameters;
	va_start(parameters, num_parameters);
	uint8_t param_arr[num_parameters];
	
	for (int i = 0; i < num_parameters; ++i) {
		param_arr[i] = va_arg(parameters, int);
	}
	va_end(parameters);
	
	usart_write_byte(packet_id);
	usart_write_byte(num_parameters);
	for (int i = 0; i < num_parameters; ++i) {
		usart_write_byte(param_arr[i]);
	}
	
}