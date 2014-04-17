/*
 * steering_commands.c
 *
 * Created: 2014-03-30 16:50:57
 *  Author: Lucas
 */ 

#include "steering_commands.h"

void drive_left_wheels(uint16_t speed)
{
	OCR1B = speed;
}

void drive_right_wheels(uint16_t speed)
{
	OCR1A = speed;
}

void drive_forward()
{

	PORTD = (1 << PORTD0) | (0 << PORTD1);
}

void drive_backwards()
{
	PORTD = (0 << PORTD0) | (1 << PORTD1);
}

void spin_right()
{
	PORTD = (1 << PORTD0) | (1 << PORTD1);
}

void spin_left()
{
	PORTD = (0 << PORTD0) | (0 << PORTD1);
}

void stop_wheels()
{
	drive_left_wheels(0);
	drive_right_wheels(0);
}

void turn_right(int speed)
{
	drive_left_wheels(speed);
	drive_right_wheels(0);
}

void turn_left(int speed)
{
	drive_left_wheels(0);
	drive_right_wheels(speed);
}

void wait_wheels(int tenth_secs)
{
	for (int i = 1; i < tenth_secs; i++)
	{
		_delay_ms(100);
	}
}

uint8_t is_driving()
{
	return (((PORTD & (1<<PORTD0)) && ~(PORTD & (1<<PORTD1))) ||
	(~(PORTD & (1<<PORTD0)) && (PORTD & (1<<PORTD1))));
}

uint8_t driving_direction()
{
	return ((PORTD & (1<<PORTD0)) && (~(PORTD & (1<<PORTD1))));
}

uint8_t is_spinning()
{
	return (((PORTD & (1<<PORTD0)) && (PORTD & (1<<PORTD1))) ||
	((~(PORTD & (1<<PORTD0))) && (~(PORTD & (1<<PORTD1)))));
}

uint8_t spinning_direction()
{
	return ((PORTD & (1<<PORTD0)) && (PORTD & (1<<PORTD1)));
}

uint8_t is_moving(uint16_t speed_left, uint16_t speed_right, uint16_t speed_compare)
{
	return ((speed_right >= speed_compare) &&
	(speed_left >= speed_compare));
}
