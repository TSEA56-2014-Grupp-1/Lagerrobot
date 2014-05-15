/*
* engine_control.c
*
* Created: 2014-03-27 10:59:57
*  Author: Erik
*/

#include "engine_control.h"
#include "Chassi.h"
#include "../shared/LCD_interface.h"
#include <stdlib.h>


void engine_set_kp(uint8_t id, uint16_t kp_data)
{
	proportional_gain = kp_data;
}

void engine_set_kd(uint8_t id, uint16_t kd_data)
{
	derivative_gain = kd_data;
	display(0, "kp %d", proportional_gain);
	display(1, "kd %d", derivative_gain);
}


void engine_init()
{
	DDRD = 0b00110011;	//Set port direction
	drive_left_wheels(1, 0);
	drive_right_wheels(1, 0);
	

	
	// set top value
	ICR1H = 0x07; //Top value high (00 with prescaler 64)  ( 0x07 with prescaler 8)
	ICR1L = 0xCF; // Top value low (249 or 0xF9 with prescaler 64) (1999 or 0x07CF with prescaler 8)
	
	// set WGM3:0 --> choose mode 14, fast pwm
	TCCR1A |= (1<< WGM11 | 0 << WGM10);
	TCCR1B |= (1 << WGM13 | 1 << WGM12);
	
	//set com1a com1b Clear OCnA/OCnB on Compare Match, set OCnA/OCnB at BOTTOM (non-inverting mode)
	TCCR1A |= (1 << COM1A1 | 0<<COM1A0 | 1<< COM1B1 | 0 << COM1B0);
	
	//set prescaler 8
	TCCR1B |= (0 << CS12 | 1 << CS11 | 0 << CS10);
}

void engine_control_command(uint8_t id, uint16_t command_data)
{
	uint8_t command = (uint8_t)command_data;

	switch(command)
	{
		//STOP WHEELS
		case CMD_STOP:
		accelerator = 0;
		steering_wheel = 0;
		manual_control = 1;
		stop_wheels();
		break;
		//Increase speed forwards
		case CMD_SPEED_UP:
		if (accelerator + STEERING_THRUST_INCREASE > STEERING_MAX_SPEED) {
			accelerator = STEERING_MAX_SPEED;
		}
		else {
			accelerator += STEERING_THRUST_INCREASE;
		}
		break;
		//Increase speed backwards, or brake in forward direction
		case CMD_SPEED_DOWN:
		if (accelerator - STEERING_THRUST_INCREASE < -STEERING_MAX_SPEED) {
			accelerator = -STEERING_MAX_SPEED;
		}
		else {
			accelerator -= STEERING_THRUST_INCREASE;
		}
		
		break;
		//Increase turningspeed to right, spin right if standing still
		case CMD_RIGHT:
		if (steering_wheel - STEERING_TURN_INCREASE < -STEERING_MAX_SPEED) {
			steering_wheel = -STEERING_MAX_SPEED;
		}
		else {
			steering_wheel -= STEERING_TURN_INCREASE;
		}
		
		break;

		//Increase turningspeed to left, spin left if standing still
		case CMD_LEFT:
		if (steering_wheel + STEERING_TURN_INCREASE > STEERING_MAX_SPEED) {
			steering_wheel = STEERING_MAX_SPEED;
		}
		else {
			steering_wheel += STEERING_TURN_INCREASE;
		}

		break;
	}

	update_steering(accelerator);

}

void drive_left_wheels(uint8_t direction, uint16_t speed)
{
	OCR1B = speed;
	if (direction == 1) {
		PORTD |= (1 << PORTD0);
	}
	else {
		PORTD &= ~(1 << PORTD0);
	}
}

void drive_right_wheels( uint8_t direction, uint16_t speed)
{
	OCR1A = speed;
	if (direction == 1) {
		PORTD &= ~(1 << PORTD1);
	}
	else {
		PORTD |= (1 << PORTD1);
	}
}

void update_steering(uint16_t speed) {
	uint16_t speed_left;
	uint16_t speed_right;
	
	int16_t velocity_left;
	int16_t velocity_right;
	
	uint8_t dir_left;
	uint8_t dir_right;
	
	velocity_left = speed - steering_wheel;
	velocity_right = speed + steering_wheel;
	
	speed_left = abs(velocity_left);
	speed_right = abs(velocity_right);
	
	if (speed_left > STEERING_MAX_SPEED) {
		speed_left = STEERING_MAX_SPEED;
	}
	
	if (velocity_left < 0) {
		dir_left = 0;
	}
	else {
		dir_left = 1;
	}
	
	if (speed_right > STEERING_MAX_SPEED) {
		speed_right = STEERING_MAX_SPEED;
	}
	
	if (velocity_right < 0) {
		dir_right = 0;
	}
	else {
		dir_right = 1;
	}
	
	drive_left_wheels(dir_left, speed_left);
	drive_right_wheels(dir_right, speed_right);
}


void stop_wheels()
{
	drive_left_wheels(1, 0);
	drive_right_wheels(1, 0);
}

