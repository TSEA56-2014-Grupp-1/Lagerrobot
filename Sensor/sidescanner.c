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
#include <stdio.h>
#include <stdlib.h>

#define ZONE_SIZE 200
#define MAX_ANGLE 140 
#define STEP 1
#define BUS_ADDRESS_ARM 2
#define DISTANCE_LOOP_COUNT 100
#define START_ANGLE 40

//XXX: uint16_t instead of int?
int compare (const void * a, const void * b)
{
	return ( *(int*)a - *(int*)b );
}

uint8_t scanner_set_position(uint8_t angle, sensor sensor_id) {
	if ((angle < 0) || (angle > 180)) {
		return 1;
	}
	else if (sensor_id == sensor_left) {
		OCR3A = SENSOR_SCANNER_ANGLE_START + SENSOR_SCANNER_ANGLE_STEP*angle;
	}
	else if (sensor_id == sensor_right) {
		OCR3B = SENSOR_SCANNER_ANGLE_START + SENSOR_SCANNER_ANGLE_STEP*angle;
	}
	else {
		return 2;
	}
	return 0;
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
	
	scanner_set_position(START_ANGLE, sensor_left);
	scanner_set_position(START_ANGLE, sensor_right);
	_delay_ms(100);
	
	//bus_register_receive(15, left_object_detection);
	//bus_register_receive(16, right_object_detection);
}

uint16_t get_distance(sensor sensor_id) {
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	ADCSRA &= ~(1 << ADIF);
	return ad_interpolate(ADC, sensor_id);
}

uint16_t get_median_distance(uint16_t angle, sensor sensor_id)
{
	scanner_set_position(angle,sensor_id);
	_delay_ms(1000);
	
	uint8_t i;
	uint16_t distance_array[DISTANCE_LOOP_COUNT];
	
	for(i = 0; i < DISTANCE_LOOP_COUNT; i++){
		distance_array[i] = get_distance(sensor_id);
	}
	
	qsort (distance_array, DISTANCE_LOOP_COUNT, sizeof(uint16_t), compare);
	return (uint16_t)distance_array[(uint8_t)(floor(DISTANCE_LOOP_COUNT/2))];
}

uint8_t find_first_distance(uint16_t *object_distance, uint16_t *object_angle, sensor sensor_id)
{	
	uint16_t distance = 400;
	uint16_t angle = 40;

	while (angle <= MAX_ANGLE)
	{
		distance = get_distance(sensor_id);
		if(distance <= ZONE_SIZE) {
			*object_distance = distance;
			*object_angle = angle;
			return 1;
		}
		else {
			angle += STEP;
			scanner_set_position(angle, sensor_id);
			_delay_ms(50);
		}
	}
	return 0;
}

uint8_t find_second_distance(uint16_t *object_distance, uint16_t *object_angle, uint16_t start_angle, sensor sensor_id) {
	
	uint16_t distance = 400;
	uint16_t angle = start_angle;
	
	while (angle <= MAX_ANGLE){
		distance = get_distance(sensor_id);
		if(distance <= ZONE_SIZE) {
			*object_angle = angle;
			*object_distance = distance;
		}
		else {
			return 1;
		}
		angle += STEP;
		scanner_set_position(angle, sensor_id);
		_delay_ms(50);
	}
	return 0;
}
			
double calculate_angle_coordinate(uint16_t angle, uint16_t distance)	{
	double alfa = angle*M_PI/180;
	double x_coord;
	double y_coord;
	x_coord = ORIGO_TO_SCANNER_DISTANCE + distance*sin(alfa);
	y_coord = distance*cos(alfa);
	return atan2(x_coord, y_coord);
}

double calculate_distance_coordinate(uint16_t angle, uint16_t distance)	{
	double alfa = angle*M_PI/180;
	double x_coord;
	double y_coord;
	x_coord = ORIGO_TO_SCANNER_DISTANCE + distance*sin(alfa);
	y_coord = distance*cos(alfa);
	return sqrt((x_coord*x_coord) + (y_coord*y_coord));
}

void object_detection(uint8_t callback_id, uint16_t meta_data)
{
	sidescanner_init();
	
	sensor sensor_id = meta_data;
	
	uint16_t first_distance = 0;
	uint16_t first_angle = 0;
	
	uint16_t second_distance = 0;
	uint16_t second_angle = 0;
	
	if (!find_first_distance(&first_distance, &first_angle, sensor_id)) {
		display(0, "No object found!");
		return;
	}
	
	if (!find_second_distance(&second_distance, &second_angle, first_angle, sensor_id)) {
		//Setting angle to max since end of the object was outside scanning area.
		second_angle = MAX_ANGLE;
		second_distance = first_distance;
	}
	
	//XXX: This should be used, not in use since debugging with display
//	uint16_t distance = get_median_distance((first_angle + second_angle)/2); 
// 	double object_angle = calculate_angle_coordinate((first_angle + second_angle)/2, distance);
// 	double object_distance = calculate_distance_coordinate(distance);

	//XXX: debugging
	double object_angle = (first_angle + second_angle)/2;
	double object_distance = get_median_distance((first_angle + second_angle)/2, sensor_id);
	
	//XXX: should be bus_transmit to arm
	display(0,"A: %d",(uint16_t)(object_angle));
	display(1,"D: %d",(uint16_t)object_distance);
}