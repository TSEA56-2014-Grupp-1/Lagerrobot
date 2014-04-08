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

int main(void) {

	//uint16_t i, j;

	servo_init();
	arm_init();

	arm_move_joint_add(2, 511);
	arm_move_joint_add(3, 511);
	arm_move_joint_add(1, 412);
	servo_action(0xfe);

	uint8_t i;
	for (i = 0;; i++) {
// 		_delay_ms(1000);
// 		arm_move_joint_add(1, 412 + 100 * (i % 3));
// 		arm_move_joint_add(5, 100 * (i % 3));
// 		arm_move_joint_add(6, 412 + 100 * (i % 3));
// 		servo_action(0xfe);
	}
}