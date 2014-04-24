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
#include <util/delay.h>

uint8_t process_arm_command(uint8_t data_length, uint8_t data[]) {
	if (!data_length) {
		return 1;
	}

	switch (data[0]) {
		case CMD_ARM_MOVE:
			if (data_length != 3) {
				return 1;
			}

			bus_transmit(
				BUS_ADDRESS_ARM, 2, ((uint16_t)data[1] << 8) | (uint16_t)data[2]);
			break;
		case CMD_ARM_GRIP:
		case CMD_ARM_RELEASE:
			// TODO: Write this
			break;
		case CMD_ARM_PREDEFINED_POS:
			if (data_length != 2) {
				return 1;
			}
			// TODO: Write this
			// XXX: data[1] == predefined position id
			break;
		case CMD_ARM_STOP:
			if (data_length != 2) {
				return 1;
			}

			bus_transmit(BUS_ADDRESS_ARM, 3, (uint16_t)data[1]);
			break;
	}

	return 0;
}

uint8_t process_chassis_command(uint8_t data_length, uint8_t data[]) {
	if (!data_length) {
		return 1;
	}

	switch (data[0]) {
		case CMD_CHASSIS_SPEED:
			// TODO: issue chassis set speed command on bus here
			break;
		case CMD_CHASSIS_STEER:
			if (data_length != 2) {
				return 1;
			}
			// TODO: issue chassis set steering power command on bus here
			// data[1] == steering power
			break;
		case CMD_CHASSIS_START:
			// TODO: issue chassis start line following / competition on bus here
			break;
		case CMD_CHASSIS_PARAMETERS:
			if (data_length != 3) {
				return 1;
			}

			// TODO: Use a forced version of transmit later and get rid of delay
			// TODO: Check status codes for transmit
			bus_transmit(BUS_ADDRESS_CHASSIS, 11, (uint16_t)data[1]); // K_p
			_delay_ms(1);
			bus_transmit(BUS_ADDRESS_CHASSIS, 12, (uint16_t)data[2]); // K_d
			break;
		case CMD_CHASSIS_MOVEMENT:
			if (data_length != 2) {
				return 1;
			}

			// Send move command
			bus_transmit(BUS_ADDRESS_CHASSIS, 8, (uint16_t)data[1]);
			break;
	}
	return 0;
}

uint8_t process_calibration_command(uint8_t data_length, uint8_t data[]) {
	if (!data_length) {
		return 1;
	}

	// XXX: Prefix constants with CMD_?
	switch (data[0]) {
		case CAL_LINE:
			if (data_length != 2) {
				return 1;
			}

			// Send 1 for tape and 0 for no tape
			// TODO: Is this redundant?
			bus_transmit(BUS_ADDRESS_SENSOR, 2, data[1]);
			break;
		case CAL_RANGE:
			if (data_length != 2) {
				return 1;
			}

			// data[1] == distance to calibration object
			// TODO: issue range sensor calibration command on bus here
			break;
	}

	return 0;
}

uint8_t process_packet_request(uint8_t data_length, uint8_t data[]) {
	uint8_t line_values[11];
	uint8_t flags;
	uint8_t center_mass;
	uint16_t temp_line_value_pair;
	uint16_t temp_flags_and_center;

	if (!data_length) {
		return 1;
	}

	switch (data[0]) {
		case PKT_LINE_DATA:
			//display(1, "Line data");

			for (int i = 0; i < 6; ++i) {
				bus_request(BUS_ADDRESS_SENSOR, 3, i, &temp_line_value_pair);
				line_values[2*i] = (uint8_t) temp_line_value_pair;
				line_values[2*i+1] = (uint8_t) (temp_line_value_pair >> 8);
			}


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
			break;
	}

	return 0;
}

uint8_t process_spoofed_request(uint8_t data_length, uint8_t data[]) {
	uint16_t response;

	if (data_length != 4) {
		return 1;
	}

	// TODO: Handle status codes?
	bus_request(
		data[0], // Address ID
		data[1], // Request ID
		((uint16_t)data[2] << 8) | (uint16_t)data[3], // Send high and low bits
		&response);

	send_packet(
		PKT_SPOOFED_RESPONSE, 2, (uint8_t)(response >> 8), (uint8_t) response);

	return 0;
}

uint8_t process_spoofed_transmit(uint8_t data_length, uint8_t data[]) {
	if (data_length != 4) {
		return 1;
	}

	// TODO: Handle status codes?
	bus_transmit(
		data[0], // Address ID
		data[1], // Transmission ID
		((uint16_t)data[2] << 8) | (uint16_t)data[3]); // Send high and low bits

	return 0;
}

uint8_t process_packet(void) {
	uint8_t packet_id;
	uint8_t packet_data_length;
	uint8_t packet_data[32]; // TODO: Verify this value
	uint8_t packet_checksum;
	uint8_t packet_calculated_checksum = 0;

	// Read packet ID
	if (usart_read_byte(&packet_id) != 0) {
		return 1;
	}
	packet_calculated_checksum += packet_id;

	// Read length of packet data
	if (usart_read_byte(&packet_data_length) != 0) {
		return 2;
	}
	packet_calculated_checksum += packet_data_length;

	// Read packet data if any
	uint8_t i;
	for (i = 0; i < packet_data_length; ++i) {
		if (usart_read_byte(&packet_data[i]) != 0) {
			return 3;
		}

		packet_calculated_checksum += packet_data[i];
	}

	// Read checksum byte and verify it
	if (usart_read_byte(&packet_checksum) != 0) {
		return 4;
	}


	if (packet_checksum != (uint8_t)~packet_calculated_checksum) {
		return 5;
	}

	// Dispatch data to appropriate function
	switch (packet_id) {
		case PKT_STOP:
			// TODO: Implement this
			break;
		case PKT_ARM_COMMAND:
			return process_arm_command(packet_data_length, packet_data);
		case PKT_CHASSIS_COMMAND:
			return process_chassis_command(packet_data_length, packet_data);
		case PKT_CALIBRATION_COMMAND:
			return process_calibration_command(packet_data_length, packet_data);
		case PKT_PACKET_REQUEST:
			return process_packet_request(packet_data_length, packet_data);
		case PKT_SPOOFED_REQUEST:
			return process_spoofed_request(packet_data_length, packet_data);
		case PKT_SPOOFED_TRANSMIT:
			return process_spoofed_transmit(packet_data_length, packet_data);
	}

	// Unknown packet ID
	return 6;
}


void send_packet(uint8_t packet_id, uint8_t num_parameters, ...) { // uint8_t parameters[]) {
	// TODO: Implement checksum calculation
	uint8_t param_arr[num_parameters];
	uint8_t checksum = 0;

	va_list parameters;
	va_start(parameters, num_parameters);

	for (int i = 0; i < num_parameters; ++i) {
		param_arr[i] = va_arg(parameters, int);
	}
	va_end(parameters);

	usart_write_byte(packet_id);
	checksum += packet_id;

	usart_write_byte(num_parameters + 1);
	checksum += num_parameters + 1;

	for (uint8_t i = 0; i < num_parameters; ++i) {
		usart_write_byte(param_arr[i]);
		checksum += param_arr[i];
	}

	usart_write_byte(~checksum);
}
