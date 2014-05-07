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
#include "../shared/LCD_interface.h"
/* qsort example */
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* qsort */


#define ZONE_SIZE 200
#define MAX_ANGLE 140 
#define STEP 1
#define BUS_ADDRESS_ARM 2
#define DISTANCE_LOOP_COUNT 100

double distance = 400;
uint16_t angle = 40;
uint8_t object_found;

int compare (const void * a, const void * b)
{
	return ( *(int*)a - *(int*)b );
}

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
	
	scanner_left_position(angle);
	scanner_right_position(angle);
	_delay_ms(100);
	
	
	//bus_register_receive(15, left_object_detection);
	//bus_register_receive(16, right_object_detection);
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


uint8_t check_distance_loop()
{
	uint8_t i;
	int distance_array[DISTANCE_LOOP_COUNT];
	//uint16_t distance_sum = 0;
	_delay_ms(50);
	
	for(i = 0; i < DISTANCE_LOOP_COUNT; i++){
		distance_array[i] = distance;
		//distance_sum += distance;
		_delay_us(500);
	}
	//ADCSRA &= (0 << ADSC);
	
	qsort (distance_array, DISTANCE_LOOP_COUNT, sizeof(int), compare);
	distance = (uint16_t)distance_array[(uint8_t)(floor(DISTANCE_LOOP_COUNT/2))];
	//distance = (distance_sum/DISTANCE_LOOP_COUNT);
	
	if(distance<=ZONE_SIZE){
		return 1;
	}
	
	else {
		ADCSRA |= (1 << ADSC);
		return 0;
	}
}

void left_object_detection (uint8_t callback_id, uint16_t data_recieved)
{
	uint16_t object_distance = 0;
	uint16_t object_angle = 0;
	
	if(sweep_left(&object_distance, &object_angle)){
		//bus_transmit(BUS_ADDRESS_ARM, 12, object_angle);
		//bus_transmit(BUS_ADDRESS_ARM, 13, object_distance);
		display(0, "Angle: %d", object_angle);
		display(1, "Dist: %d", (uint16_t)distance);
	}
	else
	{
		display(0, "No object found!");
		//bus_transmit(BUS_ADDRESS_ARM, 11, 0);
	}
	
	while(1){
		display(1, "Dist: %d", (uint16_t)distance);
		_delay_ms(500);
	}
}

void right_object_detection (uint8_t callback_id, uint16_t data_recieved)
{
	uint16_t object_distance = 0;
	uint16_t object_angle = 0;
	
	if(sweep_right(&object_distance, &object_angle)){
		//bus_transmit(BUS_ADDRESS_ARM, 14, object_angle);
		//bus_transmit(BUS_ADDRESS_ARM, 15, object_distance);
	}
	else
	{
		//bus_transmit(BUS_ADDRESS_ARM, 11, 0);
	}
}


void object_mean_angle(){
	uint16_t first_angle = angle;
	uint16_t second_angle = angle;
	
	while (angle <= MAX_ANGLE){
		if(distance <= ZONE_SIZE){
			second_angle = angle;
		}
		else {
			break;
		}
		
		angle += STEP;
		scanner_left_position(angle);
		_delay_ms(40);
		
	}
	angle = (second_angle + first_angle)/2;
	scanner_left_position(angle);
	_delay_ms(50);
}



uint8_t sweep_left(uint16_t *object_distance, uint16_t *object_angle)
{
	ADCSRA |= (1 << ADSC);
	_delay_ms(1);
	
	while (angle <= MAX_ANGLE)
	{
		//zone_size in volt = 1/distance
		if(distance<=ZONE_SIZE){
			object_mean_angle();
			if(check_distance_loop()){
			*object_distance = (uint16_t)calculate_distance_coordinate();
			*object_angle = (uint16_t)(100*calculate_angle_coordinate());
			//ADCSRA &= (0 << ADSC);
			return 1;
			}
		}
		else {
			angle += STEP;
			scanner_left_position(angle);
			_delay_ms(50);
		}
	}
	//ADCSRA &= (0 << ADSC);
	return 0;
}

uint8_t sweep_right(uint16_t *object_distance, uint16_t *object_angle)
{
	ADCSRA |= (1 << ADSC);
	_delay_ms(1);
	while (angle <= MAX_ANGLE)
	{
		//zone_size in volt = 1/distance
		if(distance<=ZONE_SIZE){
			if(check_distance_loop()){
				*object_distance = (uint16_t)calculate_distance_coordinate();
				*object_angle = (uint16_t)(100*calculate_angle_coordinate());
				ADCSRA &= (0 << ADSC);
				return 1;
			}
			else return 0;
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
	double alfa = angle*M_PI/180;
	double x_coord;
	double y_coord;
	x_coord = ORIGO_TO_SCANNER_DISTANCE + distance*sin(alfa);
	y_coord = distance*cos(alfa);
	return sqrt((x_coord*x_coord) + (y_coord*y_coord));
}
