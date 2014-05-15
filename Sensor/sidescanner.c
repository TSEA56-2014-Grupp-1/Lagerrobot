/*
 * sidescanner.c
 *
 * Created: 2014-04-01 09:30:00
 *  Author: Philip
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "sidescanner.h"
#include "../shared/LCD_interface.h"
#include "../shared/bus.h"
#include <stdio.h>
#include <stdlib.h>

#define ZONE_SIZE 250
#define STEP 1
#define DISTANCE_LOOP_COUNT 100
#define AD_CONV 20
#define ANGLE_OFFSET 0

//XXX: uint16_t instead of int?
int compare (const void * a, const void * b)
{
	return ( *(int*)a - *(int*)b );
}

uint16_t get_median_value(uint16_t array[], uint8_t size) {
	qsort (array, size, sizeof(uint16_t), compare);
	return (uint16_t)array[(uint8_t)(floor(size/2))];
}

uint8_t scanner_set_position(uint8_t angle, sensor sensor_id) {
	if ((angle < 0) || (angle > 180)) {
		return 1;
	}
	else if (sensor_id == sensor_left) {
		OCR3A = SENSOR_SCANNER_ANGLE_START + (uint16_t)(SENSOR_SCANNER_ANGLE_STEP*angle/100);
	}
	else if (sensor_id == sensor_right) {
		OCR3B = SENSOR_SCANNER_ANGLE_START + (uint16_t)(SENSOR_SCANNER_ANGLE_STEP*angle/100);
	}
	else {
		return 2;
	}
	return 0;
}

void sidescanner_init(sensor sensor_id)
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
	
	//set AD-channel 1 if sensor left, 2 if sensor right
	if (sensor_id == sensor_left)
		ADMUX = 0b00000001;
	else if(sensor_id == sensor_right)
		ADMUX = 0b00000010;
	

	scanner_set_position(SENSOR_SCANNER_ANGLE_FIRST, sensor_left);
	scanner_set_position(SENSOR_SCANNER_ANGLE_FIRST, sensor_right);
	_delay_ms(100);

	//bus_register_receive(15, left_object_detection);
	//bus_register_receive(16, right_object_detection);
}

uint16_t get_distance(sensor sensor_id) {

	uint16_t distance_array[AD_CONV];

	for (uint8_t i = 0; i < AD_CONV; ++i) {
		ADCSRA |= (1 << ADSC);
		while (ADCSRA & (1 << ADSC));
		ADCSRA &= ~(1 << ADIF);
		distance_array[i] = ADC;
	}
	return ad_interpolate(get_median_value(distance_array, AD_CONV), sensor_id) + SCANNER_AXIS_TO_FRONT;
}

uint8_t find_first_angle(uint16_t *object_angle, sensor sensor_id)
{
	uint16_t distance = 400;
	uint16_t angle = SENSOR_SCANNER_ANGLE_FIRST;

	while (angle <= SENSOR_SCANNER_ANGLE_LAST)
	{
		distance = get_distance(sensor_id);
		if(distance <= ZONE_SIZE) {
			*object_angle = angle;
			return 1;
		}
		else {
			angle += STEP;
			scanner_set_position(angle, sensor_id);
			_delay_ms(100);
		}
	}
	return 0;
}

uint8_t find_end(uint16_t *object_distance, uint16_t *object_angle, uint16_t start_angle, sensor sensor_id) {

	uint16_t distance = 400;
	uint16_t angle = start_angle;
	
	uint8_t i = 0;
	uint16_t distance_array[100];

	while (angle <= SENSOR_SCANNER_ANGLE_LAST){
		distance = get_distance(sensor_id);
		
		if(distance <= ZONE_SIZE) {
			*object_angle = angle;
			distance_array[i] = distance;
			++i;
		}
		else {
			*object_distance = get_median_value(distance_array, i+1);
			return 1;
		}
		
		angle += STEP;
		scanner_set_position(angle, sensor_id);
		_delay_ms(100);
	}
	*object_distance = get_median_value(distance_array, i+1);
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

void object_detection(sensor sensor_id)
{

	uint16_t first_angle = 0;
	uint16_t distance = 0;
	uint16_t second_angle = 0;

	if (!find_first_angle(&first_angle, sensor_id)) {
		display(0, "No object found!");
		return;
	}

	if (!find_end(&distance, &second_angle, first_angle, sensor_id)) {
		//Setting angle to max since end of the object was outside scanning area.
		second_angle = SENSOR_SCANNER_ANGLE_LAST;
		display(0, "Last angle not found");
	}

	uint16_t angle = (first_angle + second_angle)/2;

 	double object_angle = calculate_angle_coordinate(angle, distance);
 	double object_distance = calculate_distance_coordinate(angle, distance);

	uint8_t send_status;
	do {
		send_status = 0;
		send_status += bus_transmit(BUS_ADDRESS_ARM,3, (uint16_t)(object_angle*1000));
		_delay_ms(50);
		send_status += bus_transmit(BUS_ADDRESS_ARM,4, (uint16_t)object_distance);
		_delay_ms(50);
		if (send_status == 0) {
			send_status += bus_transmit(BUS_ADDRESS_ARM,5, !!distance); //send 1 if object found, 0 if not found
		}
	} while (send_status != 0);
	
	scanner_set_position(SENSOR_SCANNER_ANGLE_START, sensor_id);

}
