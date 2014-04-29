/*
 * Armenhet.c
 *
 * Created: 3/25/2014 4:23:12 PM
 *  Author: eriny778
 */

#ifndef F_CPU
	#define F_CPU 16000000UL
 #endif

#include <avr/io.h>
#include "arm.h"
#include "servo.h"
#include "inverse_kinematics.h"
#include "../shared/LCD_interface.h"
#include "../shared/bus.h"

#include <math.h>

#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>

uint16_t remote_angle = 255;
coordinate remote_coordinate = {.x = 100, .y = 100};

void arm_init()
{
	servo_write(
		SERVO_BROADCASTING_ID,
		SERVO_CW_COMPLIANCE_MARGIN,
		0x00, 0x00, 0x40, 0x40);

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

void arm_movement_command(uint8_t callback_id, uint16_t command_data)
{
	uint8_t joint = (uint8_t)(command_data);
	uint8_t direction = (uint8_t)(command_data >> 8);

	if (direction == 0)
	{
		arm_move_joint(joint, joint_get_minangle(joint));
	}
	else if(direction == 1)
	{
		arm_move_joint(joint, joint_get_maxangle(joint));
	}
}

uint8_t joint_is_moving(uint8_t joint)
{
	uint8_t i;
	uint8_t data[1];

	// Try to detect 5 times if joint is still moving before giving up and
	// assume it doesn't
	for (i = 0; i < 5; i++) {
		if (!servo_read(joint_get_servo_id(joint), SERVO_MOVING, 1, data)) {
			return data[0];
		}

		// Retry after 50 ms
		_delay_ms(50);
	}

	return 0;
}


void arm_stop_movement(uint8_t callback_id, uint16_t _joint)
{
	usart_clear_buffer();
	uint8_t data[2];
	uint16_t int_data;
	uint8_t status_code;
	uint8_t joint = (uint8_t)_joint;

	display(0, "%u", joint);

	status_code = (uint16_t)servo_read(
		joint_get_servo_id(joint),
		 SERVO_PRESENT_POSITION_L,
		  2,
		   data);

	int_data = (uint16_t)data[0] | ((uint16_t)data[1] << 8);

	display(1, "%u %u", int_data, status_code);

	arm_move_joint(joint, int_data);
	_delay_ms(30);

	while (joint_is_moving(joint) & !(status_code == 0))
	{
		usart_clear_buffer();
		status_code = (uint16_t)servo_read(joint_get_servo_id(joint), SERVO_PRESENT_POSITION_L, 2, data);
		int_data = (uint16_t)data[0] | ((uint16_t)data[1] << 8);

		arm_move_joint(joint, int_data);
		_delay_ms(30);
	}
}

void arm_move_to_angles(angles joint_angles) {
	arm_move_joint_add(2, ik_rad_to_servo_angle(2, joint_angles.t1));
	arm_move_joint_add(3, ik_rad_to_servo_angle(3, joint_angles.t2));
	arm_move_joint_add(4, ik_rad_to_servo_angle(4, joint_angles.t3));
}

void arm_process_coordinate(uint8_t callback_id, uint16_t data) {
	uint16_t value = data & 0x1ff;
	angles joint_angles;

	//display(0, "D: %u/%u/%u", callback_id, data >> 9, data & 0x1ff);
	//display(1, "Raw: %x", data);

	switch (data >> 9) {
		case 0:
			remote_coordinate.x = value;
			break;
		case 1:
			remote_coordinate.y = value;
			break;
		case 2:
			remote_angle = 2 * value;
			break;
		case 3:
			arm_move_joint(1, remote_angle);

			// Calclulate IK
			// TODO: Calculate ik_calculate_x_limit
			if (ik_angles(remote_coordinate, 150, &joint_angles) != 0) {
				return;
			}

			// while (joint_is_moving(1));



			arm_move_to_angles(joint_angles);
			servo_action(SERVO_BROADCASTING_ID);
			break;
	}
}

uint8_t arm_claw_open(void) {
	uint8_t servo_id = joint_get_servo_id(6);
	uint8_t status_code = servo_write(servo_id, SERVO_GOAL_POSITION_L, 0x00, 0x02);

	uint16_t load;

	if (status_code) {
		return status_code;
	}


	while (joint_is_moving(6)) {
		/*if (servo_read_uint16(servo_id, SERVO_PRESENT_LOAD_L, &load) == 0) {

		}*/
	}

	return 0;
}

uint8_t arm_claw_close(void) {
	uint8_t servo_id = joint_get_servo_id(6);
	uint8_t status_code = servo_write(servo_id, SERVO_GOAL_POSITION_L, 0x00, 0x00);

	uint16_t load;
	uint16_t current_position;

	if (status_code) {
		return status_code;
	}

	while (joint_is_moving(6)) {
		// Continiously check load to stop when somethings is gripped
		if (servo_read_uint16(servo_id, SERVO_PRESENT_LOAD_L, &load) == 0) {
			if ((load & 0x1ff) > 100) {
				// Display that limit was reached on servo 4
				servo_write(4, SERVO_LED, 1);

				// Disable and re-enable torque to stop claw
				servo_write(servo_id, SERVO_TORQUE_ENABLE, 0);
				//servo_write(servo_id, SERVO_TORQUE_ENABLE, 1);
				//servo_write(servo_id, SERVO_TORQUE_ENABLE, 0);

				// Clear display  on servo 4
				servo_write(4, SERVO_LED, 0);

				return 0;
			}
		}
	}

	return 0;
}

int main(void) {
	servo_init();
	arm_init();
	bus_init(BUS_ADDRESS_ARM);
	lcdi_init();

	bus_register_receive(2, arm_movement_command);
	bus_register_receive(3, arm_stop_movement);
	bus_register_receive(4, arm_process_coordinate);

	servo_write(5, SERVO_LED, 0);
	for (;;) {
		arm_claw_open();
		servo_write(5, SERVO_LED, 1);
		arm_claw_close();
		_delay_ms(1000);
	}
}
