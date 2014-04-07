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
	uint8_t num_parameters;
	uint8_t parameter;
	
	uint8_t joint;
	uint8_t pos_l;
	uint8_t pos_h;
	uint8_t speed;
	uint8_t steering_power;
	uint8_t metadata_h;
	uint8_t metadata_l;
	uint8_t predefine_id;
	uint8_t transmission_id;
	uint8_t request_id;
	uint16_t response;
	uint8_t is_on_tape;
	uint8_t distance;
	
	if (usart_read_byte(&packet_id) == 1)
		return 1;
	
// 	//if (usart_read_byte(&num_parameters) == 1)
// 		return 1;
		
	switch (packet_id) {
		case PKT_STOP:
			// issue stop command here
		break;
		case PKT_ARM_COMMAND:
			display(0, "arm command!");
			if (usart_read_byte(&parameter) == 1)
				return 1;
			switch (parameter) {
				case CMD_ARM_MOVE:
					display(0, "arm move!");
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
				break;
				case CMD_ARM_GRIP:
					// issue arm grip command on bus here
				
				break;
				case CMD_ARM_RELEASE:
					// issue arm release command on bus here
				break;
				case CMD_ARM_PREDEFINED_POS:
					
					if (usart_read_byte(&predefine_id) == 1)
						return 1;
					
					// issue predefine position command on bus here
				break;
			}
		break;
		case PKT_CHASSIS_COMMAND:
			if (usart_read_byte(&parameter) == 1)
				return 1;
				
			switch (parameter) {
				case CMD_CHASSIS_SPEED:
					
					if (usart_read_byte(&speed) == 1)
						return 1;
					
					// issue chassis set speed command on bus here
				break;
				case CMD_CHASSIS_STEER:
					
					
					if (usart_read_byte(&steering_power) == 1)
						return 1;
						
					// issue chassis set steering power command on bus here
				break;
				case CMD_CHASSIS_START:
					// issue chassis start line following / competition on bus here
				break;
			}
		break;
		case PKT_CALIBRATION_COMMAND:
			if (usart_read_byte(&parameter) == 1)
			return 1;
			switch (parameter) {
				case CAL_LINE:
					
					if (usart_read_byte(&is_on_tape) == 1)
						return 1;
					
					// issue line sensor calibration command on bus here
				break;
				case CAL_RANGE:
					
					if (usart_read_byte(&distance) == 1)
						return 1;
					
					// issue range sensor calibration command on bus here
				break;
			}
		break;
		case PKT_PACKET_REQUEST:
			if (usart_read_byte(&parameter) == 1) // parameter is packet id
				return 1;
			
			// assemble and send packet here
		break;
		case PKT_SPOOFED_REQUEST:
			
			if (usart_read_byte(&parameter) == 1	||
				usart_read_byte(&request_id) == 1	||
				usart_read_byte(&metadata_h) == 1	||
				usart_read_byte(&metadata_l) == 1)
				return 1;
			display(0, "req %d", request_id);
			bus_request(parameter, 
						request_id, 
						((uint16_t)metadata_h << 8) | (uint16_t) metadata_l, 
						&response);
						
			
			send_packet(PKT_SPOOFED_RESPONSE, 2, (uint8_t)(response >> 8), (uint8_t) response);
			
			display(1, "%x", response);
			
		break;
		case PKT_SPOOFED_TRANSMIT:	
			
			if (usart_read_byte(&parameter) == 1	||
				usart_read_byte(&transmission_id) == 1	||
				usart_read_byte(&metadata_h) == 1	||
				usart_read_byte(&metadata_l) == 1)	
			return 1;
			
			bus_transmit(parameter, 
						 transmission_id, 
						 ((uint16_t)metadata_h << 8) | (uint16_t) metadata_l);
		
		break;
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