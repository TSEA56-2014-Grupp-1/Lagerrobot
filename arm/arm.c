/*
 * Armenhet.c
 *
 * Created: 3/25/2014 4:23:12 PM
 *  Author: eriny778
 */

#if !defined(F_CPU)
	#define F_CPU 16000000UL
 #endif

#include <avr/io.h>
#include "arm.h"
#include "servo.h"
#include "../shared/LCD_interface.h"
#include "../shared/bus.h"

#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//------Functions-------

void arm_init()
{
	servo_write(
		SERVO_BROADCASTING_ID,
		SERVO_CW_COMPLIANCE_MARGIN,
		0x01, 0x01, 0x40, 0x40);

	servo_write(
		SERVO_BROADCASTING_ID,
		SERVO_GOAL_SPEED_L,
		0x40, 0x00);
}


void arm_move_joint_add(uint8_t joint, uint16_t goal_angle)
{
	switch (joint)
	{
		case 1:
			servo_move_add (0x01, goal_angle);
			break;
		case 2:
			servo_move_add (0x02, goal_angle);
			servo_move_add (0x03, 1023-goal_angle);
			break;
		case 3:
			servo_move_add (0x04, goal_angle);
			servo_move_add (0x05, 1023-goal_angle);
			break;
		case 4:
			servo_move_add (0x06, goal_angle);
			break;
		case 5:
			servo_move_add (0x07, goal_angle);
			break;
		case 6:
			servo_move_add (0x08, goal_angle);
			break;
	}
}

void arm_display_read_packet(uint8_t id, uint8_t adress, uint8_t data_length)
{
	uint8_t data[2];
	uint16_t int_data;
	(uint16_t)servo_read(id, adress, data_length, data);
	int_data = (uint16_t)data[0] | ((uint16_t)data[1] << 8);
	display(1,"%u is %u", id, int_data);
}

int main(void) {

	//uint16_t i, j;

	servo_init();
	arm_init();
	bus_init(0b0000110);
	lcd_interface_init();


// 	arm_move_joint_add(6, 511);
//   	servo_action(0xfe);
	_delay_ms(2000);
	arm_display_read_packet(3, SERVO_PRESENT_POSITION_L, 2);


	uint8_t i;
	for (i = 0;; i++) {
// 		_delay_ms(1000);
// 		arm_move_joint_add(1, 412 + 100 * (i % 3));
// 		arm_move_joint_add(5, 100 * (i % 3));
// 		arm_move_joint_add(6, 412 + 100 * (i % 3));
// 		servo_action(0xfe);
	}
}