/**
 *	@file linesensor.c
 *	@author Karl Linderhed and Philip Nilsson
 *
 *	Functions for reading and interpreting line sensor data
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "linesensor.h"
#include "../shared/bus.h"

/**
 *	Type of surface under line sensor
 */
typedef uint8_t surface_type;
enum {Floor, Tape};

/**
 *	Variable type used when handling stations, either station to the left, no
 *	station or to the right
 */
typedef uint8_t station_type;
enum station_type {Left, No, Right};

/**
 * Is set to wheter we are at a pickupstation or not, and where the station is.
 */
station_type pickup_station = No;

/**
 * The next sensor to be updated.
 */
uint8_t linesensor_channel;

/**
 * Holds the values of individual sensors.
 */
uint8_t sensor_values[11] = {0,0,0,0,0,0,0,0,0,0,0};

/**
 * The center of mass of the line.
 */
uint8_t line_weight = 127;

/**
 * Variable used in the pickup station detection
 */
uint8_t previous_pickup_station = No;

/**
 * Determines what is percieved as tape and floor. Set in linesensor_calibration().
 */
uint8_t tape_reference = 150;

/**
 * Used as a delay in pickup_staton_detection()
 */
uint16_t pickup_iterator = 0;

/**
 *	Send center of mass and station data to chassi.
 */
uint16_t send_line_data(uint8_t id, uint16_t metadata)
{
	//ADCSRA &= ~(1 << ADIE); // disable ADC-interrupt
	station_type chassi_output = 1;
	if(pickup_station == Left)
		chassi_output = Left;
	else if(pickup_station == No)
		chassi_output = No;
	else if(pickup_station == Right)
		chassi_output = Right;

	line_init();
	return (((uint16_t)(chassi_output) << 8) | line_weight);
}

/**
 *	@brief Set the tape reference to new value.
 *
 *	@param id Bus id of the function, unused.
 *	@param input_tape_reference The new value of tape_reference.
 */
uint16_t set_tape_reference(uint8_t id, uint16_t input_tape_reference)	{
	// XXX: This is never called anywhere?
	tape_reference = input_tape_reference;
	return 0;
}

/**
 *	Return current pickup station status
 */
uint8_t get_station_data()
{
	return pickup_station;
}

/**
 *	@brief Will return the values of a pair of sensors, 0 - 5.
 *
 *	@param id Bus id of the function, unused.
 *	@param sensor_pair The pair that will be returned.
 *
 *	@return High byte: value of the first sensor. Low byte: value of the second sensor.
 */
uint16_t return_linesensor(uint8_t id, uint16_t sensor_pair)	{
	if (sensor_pair != 5) {
		return ((uint16_t)sensor_values[2*sensor_pair + 1] << 8) | (uint16_t) sensor_values[2*sensor_pair];
	}
	else {
		return (uint16_t) sensor_values[2*sensor_pair];
	}

}

/**
 *	@brief Formats the output to accommodate the chassis and transmits it on the bus.
 *
 *	@param id Bus id for the function, unused.
 *	@param metadata Metadata from the bus, unused.
 *
 *	@return High byte: Station data. Low byte: Center of mass of the line.
 */
uint16_t return_line_weight(uint8_t id, uint16_t metadata)	{
	// Disable ADC interrupts

	uint8_t current_line_weight = line_weight;
	station_type chassi_output = No;
	switch (pickup_station) {
		case Left:
			chassi_output = Left;
			break;
		case Right:
			chassi_output = Right;
			break;
		default:
			// Check if we have tape at all
			if (get_tape_width() == 0) {
				chassi_output = 3;
				current_line_weight = 127;
			}
			break;
	}
	return (((uint16_t)(chassi_output) << 8) | current_line_weight);
}

/**
 *	@brief Set up ADC and direction ports for the linesensor.
 */
void line_init(){
	linesensor_channel = 0;
	ADCSRA = 0b10000111;
	DDRB = 0b11111111;
	ADMUX = (1 << ADLAR);

	PORTB = linesensor_channel;
	ADCSRA |= (1 << ADSC);
}

/**
 *	@brief Saves the current value of ADCH in sensor_values.
 */
void update_linesensor_values() {
	sensor_values[linesensor_channel] = ADCH;

	linesensor_channel = (linesensor_channel + 1) % 11;
	PORTB = linesensor_channel;

	ADCSRA |= (1 << ADSC);
}

/**
 *	@brief Check if the current sensor has tape or floor under itself.
 *
 *	@param sensor_id Sensor to check.
 *
 *	@return Tape if the sensor has tape under itself, otherwise returns floor.
 */
surface_type get_sensor_surface(uint8_t sensor_id)	{
	if(sensor_values[sensor_id] >= tape_reference)
		return Tape;
	else
		return Floor;
}

/**
 *	@brief Calculate the center of mass of the tape, will save the result in the global line_weight.
 */
void calculate_line_weight()	{
	cli();
	uint32_t temp_line_weight = 0;
	uint16_t tot_weight = 0;
	uint16_t sensor_scale = 11;
	uint8_t current_sensor = 0;

	while(current_sensor<=10)	{
		tot_weight = tot_weight + sensor_values[current_sensor];
		temp_line_weight = temp_line_weight + sensor_values[current_sensor] * sensor_scale;
		current_sensor++;
		if(current_sensor <= 10)
			sensor_scale = sensor_scale + 256/11;
	}
	if (is_tape_left() || is_tape_right())
		line_weight = 127;
	else
		line_weight = temp_line_weight / tot_weight;
	sei();
}

/**
 *	@breif Returns the width of the tape.
 *
 *	@return The width of the tape, will be integer between 0 and 11.
 */
uint8_t get_tape_width()	{
	uint8_t tape_width;
	uint8_t current_sensor;
	tape_width = 0;
	current_sensor = 0;

	while(current_sensor <=10)	{
		tape_width = tape_width + get_sensor_surface(current_sensor);
		current_sensor++;
	}
	return tape_width;
}

/**
 *	@brief Check if there is tape to mark a pickupstation right.
 *
 *	@return 1 if the 4 sensors furthest to the right has tape underneath, otherwise 0.
 */
uint8_t is_tape_right()	{
	uint8_t number_of_tape_sensors = 0;
	for(uint8_t i = 7; i<=10; i++)	{
		if (get_sensor_surface(i) == 1)
		number_of_tape_sensors++;
	}
	if(number_of_tape_sensors >= 4)
		return 1;
	else
		return 0;
}

/**
 *	@brief Check if there is tape to mark a pickupstation left.
 *
 *	@return 1 if the 4 sensors furthest to the left has tape underneath, otherwise 0.
 */
uint8_t is_tape_left()	{
	uint8_t number_of_tape_sensors = 0;
	for(uint8_t i = 0; i<5; i++)	{
		if (get_sensor_surface(i) == 1)
		number_of_tape_sensors++;
	}
	if(number_of_tape_sensors >= 4)
		return 1;
	else
		return 0;
}

/**
 *	@brief Check if there is a pickupstation or not.
 *	@detailed Will check if there is tape on either side, if there is a iterator (pickup_iterator) will start counting down.
 *		If there is tape on the opposite side on any of these iterations, it will be considered a crossing and start over.
 *		If the iterator is zero it will return pickupstation.
 */
void pickup_station_detection() {
	cli();
	if (previous_pickup_station == Left && is_tape_right()) {
		previous_pickup_station = No;
	}
	else if (previous_pickup_station == Right && is_tape_left()) {
		previous_pickup_station = No;
	}

	else if (previous_pickup_station == Left && !is_tape_left() && pickup_iterator > 1450) {
		previous_pickup_station = No;
		pickup_iterator = 0;
	}
	else if (previous_pickup_station == Right && !is_tape_right() && pickup_iterator > 1450) {
		previous_pickup_station = No;
		pickup_iterator = 0;
	}
	else if (pickup_iterator > 0) {
		--pickup_iterator;
	}

	else if (previous_pickup_station == No) {
		if (is_tape_right()) {
			previous_pickup_station = Right;
			pickup_iterator = 1500;
		}
		else if (is_tape_left()) {
			previous_pickup_station = Left;
			pickup_iterator = 1500;
		}
	}
	else if (previous_pickup_station == Left) {
		pickup_station = Left;
	}
	else if (previous_pickup_station == Right) {
		pickup_station = Right;
	}
	sei();
}

/**
 *	@brief Updates the linesensor, calculates line weight and detects pickup stations.
 */
void update_linesensor()	{
	update_linesensor_values();
	calculate_line_weight();
	pickup_station_detection();
}

/**
 *	@brief Setup ADC for calibration-routine.
 */
void init_linesensor_calibration()	{
	ADCSRA = 0b10000111;
	ADCSRA |= (1 << ADEN);
	ADMUX = 0b00100000;
	DDRB = 0b11111111;
}

/**
 *	@brief Read 10 values from one sensor, will return the average.
 *
 *	@param sensor_id ID of the sensor.
 *
 *	@return The average value of 10 readings.
 */
uint8_t line_read_sensor(uint8_t sensor_id) {
	uint16_t sensor_references = 0;
	PORTB = sensor_id;
	for (uint8_t i = 0; i <= 10; i++) {
		ADCSRA |= 0b01000000;
		while (ADCSRA &(1<<ADSC));
		ADCSRA &= ~(1 << ADIF);
		sensor_references += ADCH;
	}
	return (uint8_t)(sensor_references / 10);
}

/**
 *	@brief Calibrate the tape reference.
 *
 *	@param id Bus id of the function, unused.
 *	@param metadata Metadata to the function, unused.
 */
void calibrate_linesensor(uint8_t id, uint16_t metadata)	{
	cli();
	uint8_t sensor_max = 0;
	uint8_t sensor_min = 0;
	init_linesensor_calibration();

	uint8_t sensor_value;
	for (uint8_t j = 0; j <= 9; j++) {
		sensor_value = line_read_sensor(j);
		if (sensor_value > sensor_max) {
			sensor_max = sensor_value;
		}
		else if (sensor_value < sensor_min) {
			sensor_min = sensor_value;
		}
	}
	tape_reference = (sensor_max + sensor_min) / 2;
	ADCSRA &= ~(1 << ADIF);
	bus_transmit(BUS_ADDRESS_COMMUNICATION,13,(uint16_t)tape_reference);
	sei();
}

/**
 *	@brief Clears the pickupstation data.
 */
void clear_pickupstation() {
	pickup_station = No;
	previous_pickup_station = No;
	pickup_iterator = 0;
}


