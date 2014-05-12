/**
 *	@file servo.c
 *	@author Andreas Runfalk
 *
 *	Handles tri-state buffer control and basic servo communications. Many
 *	functions in this file should only be used in conjunction with macros defined
 *	in servo.h.
 *
 *	This library is not thread safe. No servo communication should _ever_ be done
 *	in interrupts.
 */

#include "servo.h"
#include "../shared/LCD_interface.h"
#include "../shared/bus.h"

/**
 *	Set tri-state buffer to write mode
 */
void servo_enable_write(void) {
	// XXX: Disable USART interrupts?

	// Set tri-state buffer to write
	PORTD |= 1 << PORTD2;

	// Wait for tri-state buffer to switch directions
	_delay_us(20);
}

/**
 *	Set tri-state buffer to read mode
 */
void servo_enable_read(void) {
	while (!usart_tx_complete());
	// XXX: Restore USART interrupts

	// Set tri-state buffer to read
	PORTD &= ~(1 << PORTD2);

	// Wait for tri-state buffer to switch directions
	_delay_us(20);
}

/**
 *	Initialize USART communication and tri-state buffer control
 */
void servo_init(void) {
	usart_init(SERVO_DEFAULT_BAUD_RATE);

	// Enable control of tri-state buffer and put in read mode
	DDRD |= 1 << DDD2;
	PORTD &= ~(1 << PORTD2);
}

/**
 *	Calculate servo checksum for given array from _first_index_ to _last_index_
 *
 *	@param first_index First index of array to include
 *	@param last_index Last index of array to include
 *	@param parameters[] Array to calculate checksum for
 *
 *	@return Checksum
 */
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
 *
 *	@param[in] id Servo ID that response is expected from
 *	@param[out] parameters Array that should hold response data. This must be
 *	                       big enough to fit all returned parameters.
 *
 *	@return Status code
 */
uint8_t servo_receive(uint8_t id, uint8_t *parameters) {
	uint8_t i;
	uint8_t data[256]; // XXX: This probably doesn't need to be this big
	uint8_t data_length = 0;



	// Fetch bytes into an array of length data_length
	// 0:     Start (0xff)
	// 1:     Start (0xff)
	// 2:     ID of servo
	// 3:     Number of data packets (error code + parameters + checksum)
	// 4:     Error code
	// 5-...: Parameters
	// Last:  Checksum
	for (i = 0; data_length == 0 || i < data_length; i++) {
		if (usart_read_byte(&data[i]) != 0) {
			// Read timed out
			return 1;
		}

		// Mark how many bytes to expect and pray that this is not wrong
		if (i == 3) {
			data_length = data[i] + 4;
		}
	}

	// Calculate checksum and compare against received one
	if (data[data_length - 1] != servo_calculate_checksum(2, data_length - 2, data)) {
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

	// Check if there was an error code
	if (data[4]) {
		if (data[4] & SERVO_ERROR_INSTRUCTION) {
			return 5;
		} else if (data[4] & SERVO_ERROR_OVERLOAD) {
			return 6;
		} else if (data[4] & SERVO_ERROR_CHECKSUM) {
			return 7;
		} else if (data[4] & SERVO_ERROR_RANGE) {
			return 8;
		} else if (data[4] & SERVO_ERROR_OVERHEAT) {
			return 9;
		} else if (data[4] & SERVO_ERROR_ANGLE_LIMIT) {
			return 10;
		} else if (data[4] & SERVO_ERROR_INPUT_VOLTAGE) {
			return 11;
		}
	}

	// Read parameters into given array if not null pointer
	if (data_length > 5 && parameters != 0) {
		for (i = 5; i < data_length - 1; i++) {
			parameters[i - 5] = data[i];
		}
	}

	return 0;
}
/**
 *	Transfer data to servo. To be used with variable length functions
 *
 *	@param id Servo ID
 *	@param instruction Instruction type
 *	@param data_length Number of parameters in data
 *	@param data Variable argument list of parameters to send as obtained by `va_start()`
 */
void vservo_send(
	uint8_t id, uint8_t instruction, uint8_t data_length, va_list data)
{
	uint8_t i;

	uint8_t packet_length = data_length + 6;
	uint8_t packet[packet_length];

	packet[0] = 0xff;            // Start byte
	packet[1] = 0xff;            // Start byte
	packet[2] = id;              // Servo ID
	packet[3] = data_length + 2; // Length of data + instruction + checksum
	packet[4] = instruction;     // Instruction type (read, write, ping)

	for (i = 0; i < data_length; i++) {
		packet[i + 5] = (uint8_t)va_arg(data, int);
	}

	// Add checksum to last byte
	packet[packet_length - 1] = servo_calculate_checksum(
		2, packet_length - 2, packet);

	// Disable interrupts while writing data
	cli();

	servo_enable_write();
	for (i = 0; i < packet_length; i++) {
		usart_write_byte(packet[i]);
	}

	// Indicate that all bytes are sent so servo_enable_read can wait for all
	// bytes to be transmitted before changing tri-state buffer
	usart_tx_frame();

	// Enable receiving so interrupts can be handled properly
	servo_enable_read();

	// Re-enable interrupts once write is complete
	sei();
}

/**
 *	Write data to servo. Should only be called through #servo_send()
 *
 *	@param id Servo ID to write to
 *	@param instruction Instruction type
 *	@param ... Parameters to send
 */
void _servo_send(uint8_t id, uint8_t instruction, uint8_t data_length, ...) {
	va_list data;

	va_start(data, data_length);
	vservo_send(id, instruction, data_length, data);
	va_end(data);
}

/**
 *	Send a ping packet to a servo
 *
 *	@param id Servo ID to ping
 *
 *	@return Status code like servo_receive() or 13 for illegal ID
 */
uint8_t servo_ping(uint8_t id) {
	// Verify that id isn't broadcasting ID
	if (id == SERVO_BROADCASTING_ID) {
		return 13;
	}

	// Disable interrupts while writing to servos
	cli();

	servo_send(id, SERVO_INST_PING);
	return servo_receive(id, 0);
}

/**
 *	Read an arbitrary number of bytes from the servo
 *
 *	@param[in] id Servo ID to read from
 *	@param[in] address Memory address to start reading data from
 *	@param[in] length Number of bytes to read
 *	@param[out] data Array to copy read bytes to
 *
 *	@return Status code like servo_receive() or 13 for illegal ID
 */
uint8_t servo_read(uint8_t id, uint8_t address, uint8_t length, uint8_t *data) {
	// Verify that id isn't broadcasting ID
	if (id == SERVO_BROADCASTING_ID) {
		return 13;
	}

	servo_send(id, SERVO_INST_READ, address, length);

	// TODO: Calcualte optimal delay-time!
	_delay_us(1200);
	return servo_receive(id, data);
}

/**
 *	Read one byte from a servo
 *
 *	@param[in] id Servo ID to read from
 *	@param[in] address Memory address to start reading data from
 *	@param[out] memory 8-bit integer to copy memory data to
 */
uint8_t servo_read_uint8(uint8_t id, uint8_t address, uint8_t *memory) {
	uint8_t data[1];

	uint8_t status_code = servo_read(id, address, 1, data);
	if (status_code != 0) {
		return status_code;
	}

	*memory = data[0];

	return 0;
}

/**
 *	Read two bytes from a servo. The given address should be the low end of the
 *	of the integer, for instance #SERVO_PRESENT_LOAD_L.
 *
 *	@param[in] id Servo ID to read from
 *	@param[in] address Memory address to start reading data from
 *	@param[out] memory 16-bit integer to copy memory data to
 */
uint8_t servo_read_uint16(uint8_t id, uint8_t address, uint16_t *memory) {
	uint8_t data[2];

	uint8_t status_code = servo_read(id, address, 2, data);
	if (status_code != 0) {
		return status_code;
	}

	*memory = data[0] | ((uint16_t)data[1] << 8);

	return 0;
}

/**
 *	Write data to servo. Should only be called through #servo_write()
 *
 *	@param id Servo ID to write to
 *	@param data_length Number of variable arguments
 *	@param ... Memory address followed by data bytes to write
 *
 *	@return Status code like servo_receive()
 */
uint8_t _servo_write(uint8_t id, uint8_t data_length, ...) {
	va_list data;

	va_start(data, data_length);
	vservo_send(id, SERVO_INST_WRITE, data_length, data);
	va_end(data);

	if (id == SERVO_BROADCASTING_ID) {
		return 0;
	}

	// TODO: Calculate optimal delay-time!
	_delay_us(1200);
	return servo_receive(id, 0);
}

/**
 *	Write one byte to a servo.
 *
 *	@param id Servo ID to write to
 *	@param address Memory address to write to
 *	@param data Data to write to address
 *
 *	@return Status code like servo_receive()
 */
uint8_t servo_write_uint8(uint8_t id, uint8_t address, uint8_t data) {
	return _servo_write(id, 2, address, data);
}

/**
 *	Buffer two bytes to a servo. The given address should be the low end of the
 *	of the integer, for instance #SERVO_PRESENT_LOAD_L. Bit 0 through 7 will be
 *	written to address and bit 8 through 15 to address + 1.
 *
 *	@param id Servo ID to write to
 *	@param address Starting memory address to write to
 *	@param data Data to write to address
 *
 *	@return Status code like servo_receive()
 */
uint8_t servo_write_uint16(uint8_t id, uint8_t address, uint16_t data) {
	return _servo_write(id, 3, address, (uint8_t)data, (uint8_t)(data >> 8));
}

/**
 *	Buffer write data to servo. Should only be called through #servo_reg_write()
 *
 *	@param id Servo ID to write to
 *	@param data_length Number of variable arguments
 *	@param ... Memory address followed by data bytes to write
 *
 *	@return Status code like servo_receive()
 */
uint8_t _servo_reg_write(uint8_t id, uint8_t data_length, ...) {
	va_list data;

	va_start(data, data_length);
	vservo_send(id, SERVO_INST_REG_WRITE, data_length, data);
	va_end(data);

	if (id == SERVO_BROADCASTING_ID) {
		return 0;
	}

	return servo_receive(id, 0);
}

/**
 *	Write two bytes to a servo. The given address should be the low end of the
 *	of the integer, for instance #SERVO_PRESENT_LOAD_L. Bit 0 through 7 will be
 *	written to address and bit 8 through 15 to address + 1.
 *
 *	@param id Servo ID to write to
 *	@param address Starting memory address to write to
 *	@param data Data to write to address
 *
 *	@return Status code like servo_receive()
 */
uint8_t servo_reg_write_uint16(uint8_t id, uint8_t address, uint16_t data) {
	return _servo_reg_write(id, 3, address, (uint8_t)data, (uint8_t)(data >> 8));
}

/**
 *	Buffer one byte to a servo.
 *
 *	@param id Servo ID to write to
 *	@param address Memory address to write to
 *	@param data Data to write to address
 *
 *	@return Status code like servo_receive()
 */
uint8_t servo_reg_write_uint8(uint8_t id, uint8_t address, uint8_t data) {
	return _servo_reg_write(id, 2, address, data);
}

/**
 *	Send instruction to perform all registered writes by servo_reg_write() and
 *	servo_sync_reg_write()
 */
void servo_action(uint8_t id) {
	servo_send(id, SERVO_INST_ACTION);
}
