/*
 * sidescanner.c
 *
 * Created: 2014-04-01 09:30:00
 *  Author: Philip
 */ 

#include "sidescanner.h"
#include <avr/io.h>


void sidescanner_init()
{
	DDRB = 0b11000000;	//Set port direction

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


void scanner_left_position(int angle)	//takes an angle between 0 and 180 degrees.
{
	OCR3A = SENSOR_SCANNER_ANGLE_START + SENSOR_SCANNER_ANGLE_STEP*angle;
}

void scanner_right_position(int angle)
{
	OCR3B = SENSOR_SCANNER_ANGLE_START + SENSOR_SCANNER_ANGLE_STEP*angle;
}

void scan_left_side()
{
	for (int i=0; i<180; i++)
	{
		wait_scanner_servo(50); //how fast the servo scans
		scanner_left_position(i);
	}
}

void scan_right_side()
{
	for (int i=0; i<180; i++)
	{
		wait_scanner_servo(50); 
		scanner_right_position(i);
	}
}

//wait function
void wait_scanner_servo(int milli_sec)
{
	for (int i = 0; i < milli_sec; i++)
	{
		_delay_ms(1);
	}
}
