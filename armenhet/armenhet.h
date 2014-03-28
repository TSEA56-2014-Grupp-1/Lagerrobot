#define F_CPU 16000000UL
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>


//--- Control Table Address ---
//EEPROM AREA

#define MODEL_NUMBER_L 0x0
#define MODEL_NUMBER_H 0x1
#define VERSION 0x2
#define ID 0x3
#define BAUD_RATE 0x4
#define RETURN_DELAY_TIME 0x5
#define CW_ANGLE_LIMIT_L 0x6
#define CW_ANGLE_LIMIT_H 0x7
#define CCW_ANGLE_LIMIT_L 0x8
#define CCW_ANGLE_LIMIT_H 0x9
#define SYSTEM_DATA2 0xA
#define LIMIT_TEMPERATURE 0xB
#define DOWN_LIMIT_VOLTAGE 0xC
#define UP_LIMIT_VOLTAGE 0xD
#define MAX_TORQUE_L 0xE
#define MAX_TORQUE_H 0xF
#define RETURN_LEVEL 0x10
#define ALARM_LED 0x11
#define ALARM_SHUTDOWN 0x12
#define OPERATING_MODE 0x13
#define DOWN_CALIBRATION_L 0x14
#define DOWN_CALIBRATION_H 0x15
#define UP_CALIBRATION_L 0x16
#define UP_CALIBRATION_H 0x17


//RAM-area, resets after reboot
#define TORQUE_ENABLE 0x18
#define LED 0x19
#define CW_COMPLIANCE_MARGIN 0x1A
#define CCW_COMPLIANCE_MARGIN 0x1B
#define CW_COMPLIANCE_SLOPE 0x1C
#define CCW_COMPLIANCE_SLOPE 0x1D
#define GOAL_POSITION_L 0x1E
#define GOAL_POSITION_H 0x1F
#define GOAL_SPEED_L 0x20
#define GOAL_SPEED_H 0x21
#define TORQUE_LIMIT_L 0x22
#define TORQUE_LIMIT_H 0x23
#define PRESENT_POSITION_L 0x24
#define PRESENT_POSITION_H 0x25
#define PRESENT_SPEED_L 0x26
#define PRESENT_SPEED_H 0x27
#define PRESENT_LOAD_L 0x28
#define PRESENT_LOAD_H 0x29
#define PRESENT_VOLTAGE 0x2A
#define PRESENT_TEMPERATURE 0x2B
#define REGISTERED_INSTRUCTION 0x2C
#define PAUSE_TIME 0x2D
#define MOVING 0x2E
#define LOCK 0x2F
#define PUNCH_L 0x30
#define PUNCH_H 0x31

//--- Instruction ---
#define INST_PING 0x01
#define INST_READ 0x02
#define INST_WRITE 0x03
#define INST_REG_WRITE 0x04
#define INST_ACTION 0x05
#define INST_RESET 0x06
#define INST_DIGITAL_RESET 0x07
#define INST_SYSTEM_READ 0x0C
#define INST_SYSTEM_WRITE 0x0D
#define INST_SYNC_WRITE 0x83
#define INST_SYNC_REG_WRITE 0x84

#define DEFAULT_RETURN_PACKET_SIZE 6
#define BROADCASTING_ID 0xFE

//Hardware Dependent Item
#define DEFAULT_BAUD_RATE 0x0 // 1000000bps at 16MHz

//UART control
#define CHECK_TRANSFER_FINISH (TXC0)
#define CHECK_TRANSFER_READY (!( UCSR0A & (1<<UDRE0)))
#define CHECK_TRANSMIT_FINISH (RXC0)
//Interrupt for RXD needed

#define CLEAR_BUFFER (receive_buffer_read_pointer = receive_buffer_write_pointer)


// --- Global variables ---
uint8_t parameter[128];
uint8_t transfer_buffer[128];
uint8_t recieve_buffer[128];
uint8_t recieve_interrupt_buffer[256];
uint8_t receive_buffer_read_pointer;
uint8_t receive_buffer_write_pointer;
