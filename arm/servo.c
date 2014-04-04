#include "servo.h"
#include "../shared/usart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdarg.h>

uint8_t recieve_interrupt_buffer[256];
uint8_t receive_buffer_read_pointer;
uint8_t receive_buffer_write_pointer;

void servo_init(void) {
	usart_init(SERVO_DEFAULT_BAUD_RATE);

	// Enable control of tri-state buffer
	DDRD |= 1 << DDD2;

	servo_enable_write();
}

void servo_enable_write(void) {
	// XXX: Disable USART interrupts?

	// Set tri-state buffer to read
	PORTD |= 1 << PORTD2;

	// Wait for tri-state buffer to switch directions
	_delay_us(10);
}

void servo_enable_read(void) {
	while (!usart_tx_complete());
	// XXX: Restore USART interrupts

	// Set tri-state buffer to read
	PORTD &= 0xff ^ (1 << PORTD2);

	// Wait for tri-state buffer to switch directions
	_delay_us(10);
}

uint8_t servo_calculate_checksum(uint8_t first_index, uint8_t last_index, uint8_t parameters[]) {
	uint8_t i;
	uint8_t checksum = 0;

	for (i = first_index; i <= last_index; i++) {
		checksum += parameters[i];
	}

	return ~checksum;
}

/**
 *	Read response from a servo. Possible status codes are:
 *
 *	- 0 if successful
 *	- 1 if read timed out
 *	- 2 if checksum is wrong
 *	- 3 if incorrect non 0xff start bytes
 *	- 4 if wrong servo return status
 *	- 5 if servo didn't understand the instruction
 *	- 6 if servo max torque can't handle applied load
 *	- 7 if checksum is incorrect
 *	- 8 if sent instruction is out of range
 *	- 9 if servo is overheated
 *	- 10 if goal position is out of limit range
 *	- 11 if input voltage is too low or too high
 *	-
 *
 *	@return Status code
 */
uint8_t servo_receive(uint8_t id, uint8_t *parameters) {
	uint8_t i;
	uint8_t data[256]; // XXX: This probably doesn't need to be this big
	uint8_t data_length = 0;
	servo_enable_read();

	// Fetch bytes into an array of length data_length
	for (i = 0; data_length == 0 || i < data_length; i++) {
		if (usart_read_byte(&data[i]) != 0) {
			// Read timed out
			return 1;
		}

		// Mark how many bytes to expect and pray that this is not wrong
		if (i == 3) {
			data_length = data[i] + 3;
		}
	}

	// Calculate checksum and compare against received one
	if (data[data_length - 1] != servo_calculate_checksum(2, data_length - 1, data)) {
		return 2;
	}

	// Verify that start bytes we're correct
	if (data[0] != 0xff || data[1] != 0xff) {
		return 3;
	}

	// Verify that correct servo returned data
	if (data[2] != id) {
		return 4;
	}

	if (data[3]) {
		if (data[3] & SERVO_ERROR_INSTRUCTION) {
			return 5;
		} else if (data[3] & SERVO_ERROR_OVERLOAD) {
			return 6;
		} else if (data[3] & SERVO_ERROR_CHECKSUM) {
			return 7;
		} else if (data[3] & SERVO_ERROR_RANGE) {
			return 8;
		} else if (data[3] & SERVO_ERROR_OVERHEAT) {
			return 9;
		} else if (data[3] & SERVO_ERROR_ANGLE_LIMIT) {
			return 10;
		} else if (data[3] & SERVO_ERROR_INPUT_VOLTAGE) {
			return 11;
		}
	}

	/*if (data_length > 5 && parameters != 0) {
		for (i = 4; i < data_length; i++) {
			parameters[i - 4] = data[i];
		}
	}*/

	return 0;
}

uint8_t _servo_send(
	uint8_t id, uint8_t instruction, uint8_t *parameters, uint8_t data_length, ...)
{
	uint8_t i;
	va_list data;
	uint8_t packet_length = data_length + 6;
	uint8_t packet[packet_length];

	packet[0] = 0xff;            // Start byte
	packet[1] = 0xff;            // Start byte
	packet[2] = id;              // Servo ID
	packet[3] = data_length + 2; // Length of data + instruction + checksum
	packet[4] = instruction;     // Instruction type (read, write, ping)

	va_start(data, data_length);
	for (i = 0; i < data_length; i++) {
		packet[i + 5] = (uint8_t)va_arg(data, int);
	}
	va_end(data);

	// Add checksum to last byte
	packet[packet_length - 1] = servo_calculate_checksum(
		2, packet_length - 2, packet);

	servo_enable_write();
	for (i = 0; i < packet_length; i++) {
		usart_write_byte(packet[i]);
		PORTB = packet[i];
	}

	// XXX: Move this to usart.c somehow?
	// Indicate that all bytes are sent so servo_enable_read can wait for all
	// bytes to be transmitted before changing tri-state buffer
	UCSR0A |= (1 << TXC0);

	// Check if we expect to receive data from the servo
	if (
		id != SERVO_BROADCASTING_ID ||
		instruction == SERVO_INST_PING ||
		instruction == SERVO_INST_WRITE ||
		instruction == SERVO_INST_READ)
	{
		return servo_receive(id, parameters);
	}

	return 0;
}

uint8_t servo_move(uint8_t id,  uint16_t angle) {
	// 0xff 0xff 0x01 0x05 0x03 0x1e 0x00 0x02 0xd6
	return servo_write(id, SERVO_GOAL_POSITION_L, (uint8_t)angle, (uint8_t)(angle >> 8));
}

int main(void) {
	DDRB = 0xff;
	PORTB = 0x00;
	//GICR &= 0xff ^ (1 << INT0);

	servo_init();

	uint8_t i;
	for (i = 0;; i++) {
		//servo_move(1, 412 + 100 * (i % 3));
		servo_move(1, 512);
		//PORTB = servo_move(1, 412 + (i % 3) * 100);
	}
}
