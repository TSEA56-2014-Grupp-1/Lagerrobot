/*
 * servocomm.h
 *
 * Created: 2014-03-29 16:26:03
 *  Author: Karl
 */


#ifndef SERVO_H_
#define SERVO_H_

#include <avr/io.h>

//--- Control Table Address ---
//EEPROM AREA

#define SERVO_MODEL_NUMBER_L 0x0
#define SERVO_MODEL_NUMBER_H 0x1
#define SERVO_VERSION 0x2
#define SERVO_ID 0x3
#define SERVO_BAUD_RATE 0x4
#define SERVO_RETURN_DELAY_TIME 0x5
#define SERVO_CW_ANGLE_LIMIT_L 0x6
#define SERVO_CW_ANGLE_LIMIT_H 0x7
#define SERVO_CCW_ANGLE_LIMIT_L 0x8
#define SERVO_CCW_ANGLE_LIMIT_H 0x9
#define SERVO_SYSTEM_DATA2 0xA
#define SERVO_LIMIT_TEMPERATURE 0xB
#define SERVO_DOWN_LIMIT_VOLTAGE 0xC
#define SERVO_UP_LIMIT_VOLTAGE 0xD
#define SERVO_MAX_TORQUE_L 0xE
#define SERVO_MAX_TORQUE_H 0xF
#define SERVO_RETURN_LEVEL 0x10
#define SERVO_ALARM_LED 0x11
#define SERVO_ALARM_SHUTDOWN 0x12
#define SERVO_OPERATING_MODE 0x13
#define SERVO_DOWN_CALIBRATION_L 0x14
#define SERVO_DOWN_CALIBRATION_H 0x15
#define SERVO_UP_CALIBRATION_L 0x16
#define SERVO_UP_CALIBRATION_H 0x17


//RAM-area, resets after reboot
#define SERVO_TORQUE_ENABLE 0x18
#define SERVO_LED 0x19
#define SERVO_CW_COMPLIANCE_MARGIN 0x1A
#define SERVO_CCW_COMPLIANCE_MARGIN 0x1B
#define SERVO_CW_COMPLIANCE_SLOPE 0x1C
#define SERVO_CCW_COMPLIANCE_SLOPE 0x1D
#define SERVO_GOAL_POSITION_L 0x1E
#define SERVO_GOAL_POSITION_H 0x1F
#define SERVO_GOAL_SPEED_L 0x20
#define SERVO_GOAL_SPEED_H 0x21
#define SERVO_TORQUE_LIMIT_L 0x22
#define SERVO_TORQUE_LIMIT_H 0x23
#define SERVO_PRESENT_POSITION_L 0x24
#define SERVO_PRESENT_POSITION_H 0x25
#define SERVO_PRESENT_SPEED_L 0x26
#define SERVO_PRESENT_SPEED_H 0x27
#define SERVO_PRESENT_LOAD_L 0x28
#define SERVO_PRESENT_LOAD_H 0x29
#define SERVO_PRESENT_VOLTAGE 0x2A
#define SERVO_PRESENT_TEMPERATURE 0x2B
#define SERVO_REGISTERED_INSTRUCTION 0x2C
#define SERVO_PAUSE_TIME 0x2D
#define SERVO_MOVING 0x2E
#define SERVO_LOCK 0x2F
#define SERVO_PUNCH_L 0x30
#define SERVO_PUNCH_H 0x31

//--- Instruction ---
#define SERVO_INST_PING 0x01
#define SERVO_INST_READ 0x02
#define SERVO_INST_WRITE 0x03
#define SERVO_INST_REG_WRITE 0x04
#define SERVO_INST_ACTION 0x05
#define SERVO_INST_RESET 0x06
#define SERVO_INST_DIGITAL_RESET 0x07
#define SERVO_INST_SYSTEM_READ 0x0C
#define SERVO_INST_SYSTEM_WRITE 0x0D
#define SERVO_INST_SYNC_WRITE 0x83
#define SERVO_INST_SYNC_REG_WRITE 0x84

#define SERVO_DEFAULT_RETURN_PACKET_SIZE 6
#define SERVO_BROADCASTING_ID 0xFE

#define SERVO_ERROR_INSTRUCTION (1 << 6)
#define SERVO_ERROR_OVERLOAD (1 << 5)
#define SERVO_ERROR_CHECKSUM (1 << 4)
#define SERVO_ERROR_RANGE (1 << 3)
#define SERVO_ERROR_OVERHEAT (1 << 2)
#define SERVO_ERROR_ANGLE_LIMIT (1 << 1)
#define SERVO_ERROR_INPUT_VOLTAGE 1

//Hardware Dependent Item
#define SERVO_DEFAULT_BAUD_RATE 0x0 // 1000000bps at 16MHz

// Macros for different actions
#define NUMARGS(...)  (sizeof((int[]){0, ##__VA_ARGS__}) / sizeof(int) - 1)
#define servo_send(id, instruction, parameters, ...) \
	_servo_send(id, instruction, parameters, NUMARGS(__VA_ARGS__), ##__VA_ARGS__)

#define servo_ping(id) servo_send(id, SERVO_INST_PING, (uint8_t *)(void *)0)
#define servo_read(id, address, length, parameters) \
	servo_send(id, SERVO_READ, parameters, address, length)
#define servo_write(id, ...) \
	servo_send(id, SERVO_INST_WRITE, 0, ##__VA_ARGS__)
#define servo_reg_write(id, ...) \
	servo_send(id, SERVO_INST_REQ_WRITE, 0, ##__VA_ARGS__)
#define servo_action() servo_send(SERVO_BROADCASTING_ID, SERVO_INST_ACTION, 0)

void servo_init(void);
uint8_t _servo_write()

#endif /* SERVO_H_ */
