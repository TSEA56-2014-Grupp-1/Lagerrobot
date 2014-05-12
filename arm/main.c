#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

#include <avr/io.h>

#include "arm.h"
#include "servo.h"
#include "inverse_kinematics.h"
#include "../shared/LCD_interface.h"
#include "../shared/bus.h"
#include "../shared/usart.h"

#define HEIGHT -150

/**
 *	1 if arm is currently moving
 */
uint8_t is_moving = 0;
uint8_t pickup_item = 0;

arm_coordinate pos_from_sensor;


// TODO: Redesign this
/*
void arm_movement_command(uint8_t callback_id, uint16_t command_data) {
	uint8_t joint = (uint8_t)(command_data);
	uint8_t direction = (uint8_t)(command_data >> 8);

	if (direction == 0)
	{
		arm_move(joint, joint_get_minangle(joint));
		arm_start_movement(joint);
	}
	else if(direction == 1)
	{
		arm_move(joint, joint_get_maxangle(joint));
		arm_start_movement(joint);
	}
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

	while (arm_joint_is_moving(joint) & !(status_code == 0))
	{
		usart_clear_buffer();
		status_code = (uint16_t)servo_read(joint_get_servo_id(joint), SERVO_PRESENT_POSITION_L, 2, data);
		int_data = (uint16_t)data[0] | ((uint16_t)data[1] << 8);

		arm_move_joint(joint, int_data);
		_delay_ms(30);
	}
}
*/

uint8_t move_to_remote = 0;
arm_coordinate remote_coordinate;
void arm_process_remote_coordinate(uint8_t callback_id, uint16_t data) {
	switch (callback_id) {
		case 6:
			remote_coordinate.x = data;
			break;
		case 7:
			remote_coordinate.y = (int16_t)data + ARM_FLOOR_LEVEL;
			break;
		case 8:
			remote_coordinate.angle = (float)data / 1000;
			break;
		case 9:
			remote_coordinate.angle = (float)data / -1000;
			break;
		case 10:
			move_to_remote = 1;
			break;
	}
}

void arm_receive_coord(uint8_t id, uint16_t data) {
	if (id == 3) {
		pos_from_sensor.angle = -1*(float)data / 100;
	}
	else if (id == 4) {
		pos_from_sensor.x = data;
		pos_from_sensor.y = HEIGHT;
	}
}

void sensor_done(uint8_t id, uint16_t data) {
	pickup_item = 1;
}
int main(void) {
	uint8_t i;
	uint8_t moving = 0;

	servo_init();
	arm_init();
	bus_init(BUS_ADDRESS_ARM);
	_delay_ms(1000);

	/*
	bus_register_receive(2, arm_movement_command);
	bus_register_receive(3, arm_stop_movement);
	*/
	bus_register_receive(6, arm_process_remote_coordinate);
	bus_register_receive(7, arm_process_remote_coordinate);
	bus_register_receive(8, arm_process_remote_coordinate);
	bus_register_receive(9, arm_process_remote_coordinate);
	bus_register_receive(10, arm_process_remote_coordinate);

// 	bus_register_response(1, arm_status);
// 	bus_register_receive(2, arm_movement_command);

// 	display(0, "So: %u", arm_claw_open());
// 	display(1, "Sc: %u", arm_claw_close());

	bus_register_receive(3, arm_receive_coord);
	bus_register_receive(4, arm_receive_coord);
	bus_register_receive(5, sensor_done);

	arm_claw_open();

	arm_joint_angles joint_angles;

	for (;;) {



		if (pickup_item == 1 && !ik_angles(pos_from_sensor, &joint_angles)) {
/*				display(1,"in here");*/
			joint_angles.t0 = pos_from_sensor.angle;
			arm_move_to_angles(joint_angles);
			arm_move_perform();
			pickup_item = 0;
			}
		else if (pickup_item != 0) {
			display(0, "Cord unable to reach");
			pickup_item = 0;
			}
		else if (move_to_remote) {

			display(0, "%d, %u",
				(int16_t)(remote_coordinate.angle * 1000),
				ik_joint_rad_to_angle(ARM_JOINT_BASE, remote_coordinate.angle));
			display(1, "X:%d,Y:%d",
				remote_coordinate.x,
				remote_coordinate.y);

			arm_move_to_coordinate(remote_coordinate);
			move_to_remote = 0;
		}

		// Continiously check if arm is moving
		/*moving = 0;
		for (i = 1; i <= ARM_JOINT_COUNT; ++i) {
			moving |= arm_joint_is_moving(i);
		}

		// Update global variable
		is_moving = moving;*/
	}
}
