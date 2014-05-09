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

/*
// TODO: Redesign this
void arm_movement_command(uint8_t callback_id, uint16_t command_data) {
	uint8_t joint = (uint8_t)(command_data);
	uint8_t direction = (uint8_t)(command_data >> 8);

	if (direction == 0)
	{
		arm_move(joint, joint_get_minangle(joint));
	}
	else if(direction == 1)
	{
		arm_move(joint, joint_get_maxangle(joint));
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

uint16_t remote_angle = 255;
coordinate remote_coordinate = {.x = 100, .y = 100};
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

			// while (arm_joint_is_moving(1));



			arm_move_to_angles(joint_angles);
			servo_action(SERVO_BROADCASTING_ID);
			break;
	}
}
*/

// 
// void uint16_t arm_status(uint8_t id, uint16_t data) {
// 
// }
// 
// void uint16_t arm_move(uint8_t id, uint16_t data) {
// 
// }

void arm_receive_coord(uint8_t id, uint16_t data) {
	if (id == 3) {
		pos_from_sensor.angle = -1*(float)(data)/100;
	}
	else if (id == 4) {
		pos_from_sensor.x = data;
		pos_from_sensor.y = HEIGHT;
	}
}

void sensor_done(uint8_t id, uint16_t data) {
	
	display(1,"A: %d D: %d",(int16_t)(pos_from_sensor.angle*100),(uint16_t)pos_from_sensor.x);
	
	arm_joint_angles joint_angles;
	joint_angles.t0 = pos_from_sensor.angle;
	
	if (!ik_angles(pos_from_sensor, &joint_angles)) {
		arm_move_to_angles(joint_angles);
		pickup_item = 1;
	}
	else {
		display(0, "Cord unable to reach");
		pickup_item = 0;
	}
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
	bus_register_receive(4, arm_process_coordinate);
	*/

// 	bus_register_response(1, arm_status);
// 	bus_register_receive(2, arm_movement_command);

// 	display(0, "So: %u", arm_claw_open());
// 	display(1, "Sc: %u", arm_claw_close());

	bus_register_receive(3, arm_receive_coord);
	bus_register_receive(4, arm_receive_coord);
	bus_register_receive(5, sensor_done);
	
	arm_claw_open();

	for (;;) {
		
		
		
		arm_move_perform();				
		
		if (pickup_item) {
		
			
			pickup_item = 0;
				
			// 	while(arm_joint_is_moving(ARM_JOINT_SHOULDER));
			// 	arm_claw_close();
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
