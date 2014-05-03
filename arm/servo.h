#ifndef SERVO_H_
#define SERVO_H_
/**
 *	@file servo.h
 *	@author Andreas Runfalk
 *
 *	Contains nescessary macros and constants for working with servo.c.
 *
 *	Handles tri-state buffer control and basic servo communications.
 */

#include "../shared/usart.h"
#include "../shared/numargs.h"
#include "servo_memory.h"

#if !defined(F_CPU)
	#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdarg.h>

/**
 *	@name Instruction types
 *
 *	Available types of instructions
 *
 *	@{
 */
#define SERVO_INST_PING           0x01
#define SERVO_INST_READ           0x02
#define SERVO_INST_WRITE          0x03
#define SERVO_INST_REG_WRITE      0x04
#define SERVO_INST_ACTION         0x05
#define SERVO_INST_RESET          0x06
#define SERVO_INST_DIGITAL_RESET  0x07
#define SERVO_INST_SYSTEM_READ    0x0C
#define SERVO_INST_SYSTEM_WRITE   0x0D
#define SERVO_INST_SYNC_WRITE     0x83
#define SERVO_INST_SYNC_REG_WRITE 0x84
// @}

// Possible status packet error bits
/**
 *	@name Error bits
 *
 *	All error bit meaning for return status packets
 *
 *	@{
 */
#define SERVO_ERROR_INSTRUCTION  (1 << 6)
#define SERVO_ERROR_OVERLOAD     (1 << 5)
#define SERVO_ERROR_CHECKSUM     (1 << 4)
#define SERVO_ERROR_RANGE        (1 << 3)
#define SERVO_ERROR_OVERHEAT     (1 << 2)
#define SERVO_ERROR_ANGLE_LIMIT  (1 << 1)
#define SERVO_ERROR_INPUT_VOLTAGE 1
// @}

/**
 *	Default baudrate to use. 0x0 means 1000000bps at 16MHz
 */
#define SERVO_DEFAULT_BAUD_RATE 0x0

/**
 *	Magic address to make all servos listen
 */
#define SERVO_BROADCASTING_ID 0xFE

// Helper macros for actions
/**
 *	@def servo_send
 *	@brief Send arbitrary command to servo
 *
 *	Base function for sending data to servos. Uses _servo__send() to send specified
 *	data. Calling function is responsible for calling servo_receive() when1there
 *	is return data.
 *
 *	@param id Servo ID
 *	@param instruction Instruction type
 *	@param ... Parameters to send
 */
#define servo_send(id, instruction, ...) \
	_servo_send(id, instruction, NUMARGS(__VA_ARGS__), ##__VA_ARGS__)

/**
 *	@def servo_write
 *	@brief Write to servo
 *
 *	Uses _servo_write() to write data to the specified servo
 *
 *	@param id Servo ID
 *	@param address Starting address to write to
 *	@param ... Bytes to write
 *
 *	@return Status code as defined by servo_receive()
 */
#define servo_write(id, address, ...) \
	_servo_write(id, NUMARGS(__VA_ARGS__) + 1, address, ##__VA_ARGS__)

/**
 *	@def servo_reg_write
 *	@brief Buffered write to servo
 *
 *	Uses _servo_reg_write() to write data to the specified servo. Servo will wait
 *	to perform the write until it receives an action instruction. Action
 *	instructions can be sent using servo_action()
 *
 *	@param id Servo ID
 *	@param address Starting address to write to
 *	@param ... Bytes to write
 *
 *	@return Status code as defined by servo_receive()
 */
#define servo_reg_write(id, address, ...) \
	_servo_reg_write(id, NUMARGS(__VA_ARGS__) + 1, address, ##__VA_ARGS__)

/**
 *	@def servo_sync_write
 *
 *	Write to multiple servos using one command.
 *
 *	@param address Start address to write to
 *	@param length Number of bytes to write to each servo
 *	@param ... Servo ID followed by _length_ arguments, followed by servo ID again...
 */
#define servo_sync_write(address, length, ...) \
	servo_send(SERVO_BROADCASTING_ID, SERVO_INST_SYNC_WRITE, address, length, __VA_ARGS__)

/**
 *	@def servo_sync_reg_write
 *	@brief Buffered write to servo multiple servos
 *
 *	Buffered write to multiple servos using one command. Servo will wait
 *	to perform the write until it receives an action instruction. Action
 *	instructions can be sent using servo_action()
 *
 *	@param address Start address to write to
 *	@param length Number of bytes to write to each servo
 *	@param ... Servo ID followed by _length_ arguments, followed by servo ID again...
 */
#define servo_sync_reg_write(address, length, ...) \
	servo_send(SERVO_BROADCASTING_ID, SERVO_INST_SYNC_REG_WRITE, address, length, __VA_ARGS__)

void servo_init(void);

uint8_t servo_ping(uint8_t id);
uint8_t servo_read(uint8_t id, uint8_t address, uint8_t length, uint8_t *data);
uint8_t servo_read_uint8(uint8_t id, uint8_t address, uint8_t *memory);
uint8_t servo_read_uint16(uint8_t id, uint8_t address, uint16_t *memory);
uint8_t _servo_write(uint8_t, uint8_t, ...);
uint8_t _servo_reg_write(uint8_t, uint8_t, ...);
uint8_t servo_move_add(uint8_t id, uint16_t angle);
uint8_t servo_write_uint8(uint8_t id, uint8_t address, uint8_t data);
uint8_t servo_write_uint16(uint8_t id, uint8_t address, uint16_t data);
void servo_action(uint8_t id);
uint8_t servo_move(uint8_t id, uint16_t angle);

#endif /* SERVO_H_ */
