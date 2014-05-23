/**
 *	@file enigine_control.c
 *	@author Karl Linderhed
 *
 *	Engine control code. Can individually control both sides of wheels.
 */

#include "engine_control.h"
#include "Chassi.h"
#include "../shared/LCD_interface.h"
#include "../shared/utils.h"
#include <stdlib.h>

/**
 *	Current steering direction
 */
int16_t current_steering_wheel = 0;

/**
 *	Current accelerator amount
 */
int16_t current_accelerator = 0;

void engine_init(void)
{
	DDRD = 0b00110011;	//Set port direction
	drive_left_wheels(1, 0);
	drive_right_wheels(1, 0);

	current_steering_wheel = 0;
	current_accelerator = 0;

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

/**
 *	Read manual steering commands
 */
void engine_control_command(uint8_t id, uint16_t command_data)
{
	uint8_t command = (uint8_t)command_data;

	int16_t accelerator = get_speed();
	int16_t steering_wheel = get_steering_wheel();

	switch(command)
	{
		//STOP WHEELS
		case CMD_STOP:
			accelerator = 0;
			steering_wheel = 0;
			manual_control = 1; // XXX: Why?
			stop_wheels();
			break;
		//Increase speed forwards
		case CMD_SPEED_UP:
			accelerator = MIN(
				STEERING_MAX_SPEED, accelerator + STEERING_THRUST_INCREASE);
			break;
		//Increase speed backwards, or brake in forward direction
		case CMD_SPEED_DOWN:
			accelerator = MAX(
				-STEERING_MAX_SPEED, accelerator - STEERING_THRUST_INCREASE);
			break;
		//Increase turningspeed to right, spin right if standing still
		case CMD_RIGHT:
			steering_wheel = MAX(
				-STEERING_MAX_SPEED, steering_wheel - STEERING_TURN_INCREASE);
			break;
		//Increase turningspeed to left, spin left if standing still
		case CMD_LEFT:
			steering_wheel = MIN(
				STEERING_MAX_SPEED, steering_wheel + STEERING_TURN_INCREASE);
			break;
	}

	update_steering(steering_wheel, accelerator);
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

void drive_right_wheels(uint8_t direction, uint16_t speed)
{
	OCR1A = speed;
	if (direction == 1) {
		PORTD &= ~(1 << PORTD1);
	}
	else {
		PORTD |= (1 << PORTD1);
	}
}

void update_steering(int16_t steering_wheel, uint16_t accelerator)
{
	uint16_t speed_left;
	uint16_t speed_right;

	int16_t velocity_left;
	int16_t velocity_right;

	uint8_t dir_left;
	uint8_t dir_right;

	// Update reference steering and accelerator
	current_steering_wheel = steering_wheel;
	current_accelerator = accelerator;

	velocity_left = accelerator - steering_wheel;
	velocity_right = accelerator + steering_wheel;

	speed_left = MIN(abs(velocity_left), STEERING_MAX_SPEED);
	speed_right = MIN(abs(velocity_right), STEERING_MAX_SPEED);

	if (velocity_left < 0) {
		dir_left = 0;
	}
	else {
		dir_left = 1;
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


void stop_wheels(void)
{
	drive_left_wheels(1, 0);
	drive_right_wheels(1, 0);
}

int16_t get_steering_wheel(void)
{
	return current_steering_wheel;
}

int16_t get_speed(void)
{
	return current_accelerator;
}
