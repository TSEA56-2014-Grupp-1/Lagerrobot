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


#define ZONE_SIZE 200
#define MAX_ANGLE 140 
#define STEP 1

double distance = 400;
uint8_t angle = 40;
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
	ADCSRA = 0b10011111;
	//set AD-channel 1
	ADMUX = 0b00000001;	
	//Start AD-conversion
	
	bus_register_receive(15, left_object_detection);
	bus_register_receive(16, right_object_detection);
}


uint8_t scanner_left_position(uint8_t angle)
{
	if((0<=angle) && (angle<=180)){
	OCR3A = SENSOR_SCANNER_ANGLE_START + SENSOR_SCANNER_ANGLE_STEP*angle;
	return 0;
	}
	else
	return 1;

}

uint8_t scanner_right_position(uint8_t angle)
{
	if((0<=angle) && (angle<=180)){
	OCR3B = SENSOR_SCANNER_ANGLE_START + SENSOR_SCANNER_ANGLE_STEP*angle;
	return 0;
	}
	else
	return 1;

}


void update_distance_sensor_2()	{
	distance = ad_interpolate(ADC,2);
	ADCSRA |= (1 << ADSC);
}

void update_distance_sensor_3()	{
	distance = ad_interpolate(ADC,3);
	ADCSRA |= (1 << ADSC);
}


void left_object_detection (uint8_t callback_id, uint16_t data_recieved)
{
	uint8_t object_distance;
	uint8_t object_angle;
	
	if(sweep_left(object_distance*, object_angle*)){
		bus_transmit(BUS_ADDRESS_ARM, 12, object_angle);
		bus_transmit(BUS_ADDRESS_ARM, 13, object_distance);
	}
	else
	{
		bus_transmit(BUS_ADDRESS_ARM, 11, 0);
	}
}

void right_object_detection (uint8_t callback_id, uint16_t data_recieved)
{
	uint8_t object_distance;
	uint8_t object_angle;
	
	if(sweep_right(object_distance*, object_angle*)){
		bus_transmit(BUS_ADDRESS_ARM, 14, object_angle);
		bus_transmit(BUS_ADDRESS_ARM, 15, object_distance);
	}
	else
	{
		bus_transmit(BUS_ADDRESS_ARM, 11, 0);
	}
}

uint8_t sweep_left(uint16_t object_distance, uint16_t object_angle)
{
	ADCSRA |= (1 << ADSC);
	_delay_ms(1);
	
	while (angle <= MAX_ANGLE)
	{
		//zone_size in volt = 1/distance
		if(distance<=ZONE_SIZE){
			object_distance = (uint16_t)calculate_distance_coordinate();
			object_angle = (uint16_t)(100*calculate_angle_coordinate());
			ADCSRA &= (0 << ADSC);
			return 1;
		}
		else {
			angle += STEP;
			scanner_left_position(object_angle);
			_delay_ms(50);
		}
	}
	ADCSRA &= (0 << ADSC);
	return 0;
}

uint8_t sweep_right(uint16_t object_distance, uint16_t object_angle)
{
	ADCSRA |= (1 << ADSC);
	_delay_ms(1);
	while (angle <= MAX_ANGLE)
	{
		//zone_size in volt = 1/distance
		if(distance<=ZONE_SIZE){
			object_distance = (uint16_t)calculate_distance_coordinate();
			object_angle = (uint16_t)(100*calculate_angle_coordinate());
			ADCSRA &= (0 << ADSC);
			return 1;
		}
		else {
			angle += STEP;
			scanner_right_position(angle);
			_delay_ms(50);
		}
	}
	ADCSRA &= (0 << ADSC);
	return 0;
	
}

double calculate_angle_coordinate()	{
	double alfa = angle*M_PI/180;
	double x_coord;
	double y_coord;
	x_coord = ORIGO_TO_SCANNER_DISTANCE + distance*sin(alfa);
	y_coord = distance*cos(alfa);
	return atan2(x_coord,y_coord);
}

double calculate_distance_coordinate()	{
	double alfa = (angle - 90)*M_PI/180;
	double x_coord;
	double y_coord;
	x_coord = ORIGO_TO_SCANNER_DISTANCE + distance*sin(alfa);
	y_coord = distance*cos(alfa);
	return sqrt((x_coord*x_coord) + (y_coord*y_coord));
}
