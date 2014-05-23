/**
 *	@file sidescanner.c
 *	@author Johan Lind and Philip Nilsson
 *
 *	Functions for handling robot side scanners
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "sidescanner.h"
#include "../shared/LCD_interface.h"
#include "../shared/bus.h"
#include <stdio.h>
#include <stdlib.h>

/**
 *	Maximum distance which is considered to be object
 */
#define ZONE_SIZE 300

/**
 *	Number of steps to take for each increment
 */
#define STEP 1

/**
 *	Number of AD conversions to make for each step
 */
#define AD_CONV 20

/**
 *	Comparison function for two integers A and B. If A < B a negative value will
 *	be returned, if A > B a positive value will be returned, else 0 will be
 *	returned.
 */
int compare (const void * a, const void * b)
{
	return ( *(int*)a - *(int*)b );
}

/**
 *	Calculate median values of given array.
 *
 *	@param array[] Array of integers
 *	@param size Number elements in array
 *
 *	@return Median value
 */
uint16_t get_median_value(uint16_t array[], uint8_t size) {
	// XXX: Does not properly handle even sized arrays
	qsort (array, size, sizeof(uint16_t), compare);
	return (uint16_t)array[(uint8_t)(floor(size/2))];
}

/**
 *	Set scanner to a position. This does not block until move is complete.
 *	Possible status codes are:
 *
 *	- 0 if successful
 *	- 1 if invalid angle (not between 0 and 180)
 *	- 2 if invalid sensor ID
 *
 *	@param angle Angle in degrees to move servo to
 *	@param sensor_id ID of sensor to move
 *
 *	@return Status code
 */
uint8_t scanner_set_position(uint8_t angle, sensor sensor_id) {
	if ((angle < 0) || (angle > 180)) {
		return 1;
	}
	else if (sensor_id == sensor_left) {
		OCR3A = SENSOR_SCANNER_LEFT_ANGLE_START + (uint16_t)(SENSOR_SCANNER_LEFT_ANGLE_STEP*angle/100);
	}
	else if (sensor_id == sensor_right) {
		OCR3B = SENSOR_SCANNER_RIGHT_ANGLE_START + (uint16_t)(SENSOR_SCANNER_RIGHT_ANGLE_STEP*angle/100);
	}
	else {
		return 2;
	}
	return 0;
}

/**
 *	Set up registers for side scanner control for the given ID and make sure it
 *	is in a sane default position.
 *
 *	@param sensor_id Sensor ID for sidescanner to initialize
 */
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
	ADCSRA = 0b10010111;

	//set AD-channel 1 if sensor left, 2 if sensor right
	if (sensor_id == sensor_left)
		ADMUX = 0b00000001;
	else if(sensor_id == sensor_right)
		ADMUX = 0b00000010;


	// Use sane default positions
	scanner_set_position(SENSOR_SCANNER_ANGLE_FIRST, sensor_left);
	scanner_set_position(SENSOR_SCANNER_ANGLE_FIRST, sensor_right);

	// Wait for servos to move into position
	_delay_ms(100);
}

/**
 *	Measure distance from rotation axis in mm for the given sensor.
 *
 *	@param sensor_id Sensor to measure
 *
 *	@return Distance from rotation axis in mm
 */
uint16_t get_distance(sensor sensor_id) {

	uint16_t distance_array[AD_CONV];
	uint16_t distance_value;
	for (uint8_t i = 0; i < AD_CONV; ++i) {
		ADCSRA |= (1 << ADSC);
		while (ADCSRA & (1 << ADSC));
		ADCSRA &= ~(1 << ADIF);
		distance_array[i] = ADC;
	}

	distance_value = ad_interpolate(get_median_value(distance_array, AD_CONV), sensor_id) + SCANNER_AXIS_TO_FRONT;
	while (bus_transmit(BUS_ADDRESS_COMMUNICATION, 10, ((uint16_t) sensor_id << 10) | (distance_value & 0b01111111111)));
	return distance_value;
}

/**
 *	Move servo from starting position until an object is found. Possible status
 *	codes are:
 *
 *	- 0 if object was found
 *	- 1 if no object was found
 *
 *	@param[out] object_angle Angle to write to
 *	@param[in] sensor_id Sensor to scan with
 *
 *	@return status code
 */
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

/**
 *	Move servo from detected object edge until a new distance is found position until an object is found. Possible status
 *	codes are:
 *
 *	- 0 if scanning range was exausted before object end was detected
 *	- 1 if no object was found
 *
 *	@param[out] object_distance Angle to write to
 *	@param[out] object_angle Angle to write to
 *	@param[in] start_angle Start angle of object to detect end for
 *	@param[in] sensor_id Sensor to scan with
 *
 *	@return status code
 */
uint8_t find_end(uint16_t *object_distance, uint16_t *object_angle, uint16_t start_angle, sensor sensor_id) {

	uint16_t distance = 400;
	uint16_t angle = start_angle;

	uint8_t i = 0;
	uint16_t distance_array[100]; // XXX: This may be a potential crash source

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

/**
 *	Convert scanner angle and distance to a arm relative angle
 *
 *	@param angle Angle in degrees
 *	@param distance Distance in mm as returned by scanner
 *	@param sensor_id Sensor ID to convert angle for
 *
 *	@return Angle in radians relative to arm
 */
double calculate_angle_coordinate(uint16_t angle, uint16_t distance, sensor sensor_id)	{
	double alfa = angle * M_PI / 180;
	double x_coord;
	double y_coord;

	x_coord = distance * sin(alfa);
	y_coord = distance * cos(alfa);

	if (sensor_id == sensor_left) {
		x_coord += ORIGO_TO_SCANNER_DISTANCE_LEFT;
	} else {
		x_coord += ORIGO_TO_SCANNER_DISTANCE_RIGHT;
	}

	return atan2(x_coord, y_coord);
}

/**
 *	Convert scanner angle and distance to a arm relative distance
 *
 *	@param angle Angle in degrees
 *	@param distance Distance in mm as returned by scanner
 *	@param sensor_id Sensor ID to convert angle for
 *
 *	@return Distance from arm center in mm
 */
double calculate_distance_coordinate(uint16_t angle, uint16_t distance, sensor sensor_id)	{
	double alfa = angle * M_PI / 180;
	double x_coord;
	double y_coord;

	x_coord = distance * sin(alfa);
	y_coord = distance * cos(alfa);

	if (sensor_id == sensor_left) {
		x_coord += ORIGO_TO_SCANNER_DISTANCE_LEFT;
	} else {
		x_coord += ORIGO_TO_SCANNER_DISTANCE_RIGHT;
	}

	return sqrt((x_coord*x_coord) + (y_coord*y_coord));
}

/**
 *	Initiate scanning procedure. When object is found or not arm is messaged with
 *	result. This function blocks until scanning is done.
 *
 *	@param sensor_id Sensor to scan with
 */
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

	if (sensor_id == sensor_right) {
		angle = 180 - angle;
	}

	double object_angle = calculate_angle_coordinate(angle, distance, sensor_id);
	double object_distance = calculate_distance_coordinate(angle, distance, sensor_id);

	uint8_t send_status;
	scanner_set_position(SENSOR_SCANNER_ANGLE_FIRST, sensor_left);
	scanner_set_position(SENSOR_SCANNER_ANGLE_FIRST, sensor_right);
	do {
		send_status = 0;
		send_status += bus_transmit(BUS_ADDRESS_ARM,3 , (uint16_t)(object_angle * 500));
		_delay_ms(50);
		send_status += bus_transmit(BUS_ADDRESS_ARM,4 , (uint16_t)object_distance);
		_delay_ms(50);
		if (send_status == 0) {
			// Send 1 if object found, 0 if not found so arm can move on
			send_status += bus_transmit(BUS_ADDRESS_ARM, 5, !!distance);
		}
	} while (send_status != 0);
}
