/*
 * sidescanner.c
 *
 * Created: 2014-04-01 09:30:00
 *  Author: Philip
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "sidescanner.h"
//constants
uint8_t start_angle = 156;
uint16_t end_angle = 780;
double step = (780 - 156) / 180;


uint8_t distance;
uint8_t zone_size;
uint8_t angle;
uint8_t max_angle = 180;
uint8_t object_found;
uint8_t step;
uint8_t distance_read_ok;

void sidescanner_init()
{
	DDRB = 0b01100000;	//Set port direction

	// set top value
	ICR3H = 0x18; //Top value high (18 with prescaler 64)
	ICR3L = 0x69; // Top value low (6249 or 0x1869 with prescaler 64)

	// set WGM3:0 --> choose mode 14, fast pwm
	TCCR3A |= (1<< WGM11 | 0 << WGM10);
	TCCR3B |= (1 << WGM13 | 1 << WGM12);

	//set com1a com1b Clear OCnA/OCnB on Compare Match, set OCnA/OCnB at BOTTOM (non-inverting mode)
	TCCR3A |= (1 << COM1A1 | 0<<COM1A0 | 1<< COM1B1 | 0 << COM1B0);

	//set prescaler 64
	TCCR3B |= (0 << CS12 | 1 << CS11 | 1 << CS10);
}


uint8_t scanner_left_position(uint8_t angle)
{
	if(angle<=180)
	OCR3A = start_angle + step*angle;
	else
	return 1;
	return 0;
}

uint8_t scanner_right_position(uint8_t angle)
{
	if(angle<=180)
	OCR3B = start_angle + step*angle;
	else
	return 1;
	return 0;
}

void update_distance()	{
	distance = ADCH;
	distance_read_ok = 1;
}

uint8_t sweep_left()	{
	if(distance<=zone_size)	{
		object_found = 1;
	}
	else if(angle == max_angle)	{
		return 1;
	}
	else
	{
		object_found = 0;
		angle = angle + step;
		scanner_left_position(angle);
	}
	return 0;
}

uint8_t sweep_right()	{
	
}