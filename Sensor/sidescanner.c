/*
 * sidescanner.c
 *
 * Created: 2014-04-01 09:30:00
 *  Author: Philip
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "sidescanner.h"
#include "distance_sensors.h"

double distance = 400;
#define ZONE_SIZE 200
#define MAX_ANGLE 180 
#define STEP 1

uint8_t angle = 0;
uint8_t object_found;

void sidescanner_init()
{
	DDRB |= 0b11000000;	//Set port direction
	
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
	
	//Enable adc, Set ADIF flag, Set ADC Interreupt enable, set prescaler
	ADCSRA = 0b10001111;
	//Set left AD-left adjust, set AD-channel 1
	ADMUX |= 0b00000001;	
	//Start AD-conversion
	ADCSRA |= (1 << ADSC);
	
}


uint8_t scanner_left_position(uint8_t angle)
{
	if((0<=angle) && (angle<=180))
	OCR3A = SENSOR_SCANNER_ANGLE_START + SENSOR_SCANNER_ANGLE_STEP*angle;
	else
	return 1;
	return 0;
}

uint8_t scanner_right_position(uint8_t angle)
{
	if((0<=angle) && (angle<=180))
	OCR3B = SENSOR_SCANNER_ANGLE_START + SENSOR_SCANNER_ANGLE_STEP*angle;
	else
	return 1;
	return 0;
}


void update_distance_sensor_2()	{
	distance = ad_interpolate(ADC,2);
	ADCSRA |= (1 << ADSC);
}

void update_distance_sensor_3()	{
	distance = ad_interpolate(ADC,3);
	ADCSRA |= (1 << ADSC);
}

uint8_t sweep_left()	{
	//zone_size in volt = 1/distance
	if(distance<=ZONE_SIZE)	
	{
		object_found = 1;
		scanner_left_position(angle);
		//wait_scanner_servo(5000);
	}
	else if(angle == MAX_ANGLE)	{
		return 1;
	}
	else
	{
		object_found = 0;
		angle = angle + STEP;
		scanner_left_position(angle);
		wait_scanner_servo(50); 
	}
	return 0;
}

uint8_t sweep_right()	{
		if(distance>=ZONE_SIZE)
		{
			object_found = 1;
			scanner_right_position(0);
		}
		else if(angle == MAX_ANGLE)	{
			return 1;
		}
		else
		{
			object_found = 0;
			angle = angle + STEP;
			scanner_right_position(angle);
			wait_scanner_servo(50);
		}
	return 0;
}

void wait_scanner_servo(int milli_sec)
{
	for (int i = 0; i < milli_sec; i++)
	{
		_delay_ms(1);
	}
}

uint8_t calculate_angle_coordinate()	{
	double alfa = (angle - 90)*M_PI/180;
	uint16_t x_coord;
	uint16_t y_coord;
	x_coord = ORIGO_TO_SCANNER_DISTANCE + distance*cos(alfa);
	y_coord = distance*sin(alfa);
	return tan(y_coord/x_coord);
}

uint8_t calculate_distance_coordinate()	{
	double alfa = (angle - 90)*M_PI/180;
	uint16_t x_coord;
	uint16_t y_coord;
	x_coord = ORIGO_TO_SCANNER_DISTANCE + distance*cos(alfa);
	y_coord = distance*sin(alfa);
	return sqrt((x_coord^2) + (y_coord^2));
}
