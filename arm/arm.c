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

void arm_move_joint(uint8_t joint, uint16_t goal_angle)
{
	switch (joint)
	{
		case 1:
		servo_move (0x01, goal_angle);
		break;
		case 2:
		servo_move_add (0x02, goal_angle);
		servo_move_add (0x03, 1023-goal_angle);
		servo_action(0xfe);
		break;
		case 3:
		servo_move_add (0x04, goal_angle);
		servo_move_add (0x05, 1023-goal_angle);
		servo_action(0xfe);
		break;
		case 4:
		servo_move (0x06, goal_angle);
		break;
		case 5:
		servo_move (0x07, goal_angle);
		break;
		case 6:
		servo_move (0x08, goal_angle);
		break;
	}
}

void arm_display_read_packet(uint8_t id, uint8_t adress, uint8_t length)
{
	uint8_t data[2];
	uint16_t int_data;
	int_data = (uint16_t)servo_read(id, adress, length, data);
	if (length == 1)
	{
		display(1,"%u is %u", id, int_data);
	}
	else
	{
		int_data = (uint16_t)data[0] | ((uint16_t)data[1] << 8);
		display(1,"%u is %u", id, int_data);
	}
}

uint16_t joint_get_minangle(uint8_t joint)
{
	switch(joint)
	{
		case 1:
			return JOINT_1_MINANGLE;
		case 2:
			return JOINT_2_MINANGLE;
		case 3:
			return JOINT_3_MINANGLE;
		case 4:
			return JOINT_4_MINANGLE;
		case 5:
			return JOINT_5_MINANGLE;
		case 6:
			return JOINT_6_MINANGLE;
	}
	return 0;
}

uint16_t joint_get_maxangle(uint8_t joint)
{
	switch(joint)
	{
		case 1:
			return JOINT_1_MAXANGLE;
		case 2:
			return JOINT_2_MAXANGLE;
		case 3:
			return JOINT_3_MAXANGLE;
		case 4:
			return JOINT_4_MAXANGLE;
		case 5:
			return JOINT_5_MAXANGLE;
		case 6:
			return JOINT_6_MAXANGLE;
	}
	return 0;
}

uint8_t joint_get_servo_id(uint8_t joint)
{
		switch(joint)
		{
			case 1:
				return 1;
			case 2:
				return 2;
			case 3:
				return 4;
			case 4:
				return 6;
			case 5:
				return 7;
			case 6:
				return 8;
		}
	return 0;
}

void arm_movement_command(uint8_t joint, uint8_t direction)
{
	if (direction == 0)
	{
		arm_move_joint(joint, joint_get_minangle(joint));
	}
	else if(direction == 1)
	{
		arm_move_joint(joint, joint_get_maxangle(joint));
	}
}

uint8_t arm_is_joint_moving(uint8_t joint)
{
	uint8_t data[1];
	servo_read(joint_get_servo_id(joint), SERVO_MOVING, 1, data);
	return data[0];
}


void arm_stop_movement(uint8_t joint)
{
	uint8_t data[2];
	uint16_t int_data;
	uint8_t status_code;



	status_code = (uint16_t)servo_read(joint_get_servo_id(joint), SERVO_PRESENT_POSITION_L, 2, data);
	int_data = (uint16_t)data[0] | ((uint16_t)data[1] << 8);

	arm_move_joint(joint, int_data);
	_delay_ms(30);
	
	while (arm_is_joint_moving(joint) & !(status_code == 0))
	{
		display(1, "%u is %u", joint_get_servo_id(joint), status_code);
		usart_clear_buffer();
		status_code = (uint16_t)servo_read(joint_get_servo_id(joint), SERVO_PRESENT_POSITION_L, 2, data);
		int_data = (uint16_t)data[0] | ((uint16_t)data[1] << 8);

		arm_move_joint(joint, int_data);
		_delay_ms(30);
	}
	
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

	uint8_t i;
	for (i = 0;; i++) {
		_delay_ms(1000);
		arm_movement_command(1, i%2);
		arm_movement_command(5, i%2);
		arm_movement_command(6, i%2);
		_delay_ms(1000);
		arm_stop_movement(1);
		arm_stop_movement(5);
		arm_stop_movement(6);
	}
}