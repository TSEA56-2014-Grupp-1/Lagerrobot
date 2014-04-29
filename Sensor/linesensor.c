/*
 * linesensor.c
 *
 * Created: 2014-03-27 09:08:16
 *  Author: Karl
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "linesensor.h"
#include "../shared/bus.h"
#include "../RFID_scanner/RFID_scanner.h"
#define F_CPU 20000000UL
#include <util/delay.h>
//#include "../shared/LCD_interface.h"


//Defining different datatypes	
typedef uint8_t surface_type;
enum {Floor, Tape};

typedef uint8_t station_type;
enum station_type {Left, No, Right};

typedef uint8_t composite_output_type;
enum composite_output_type {station_Left, station_No, station_Right,No_tape};

station_type pickup_station = No;
uint8_t linesensor_channel;
uint8_t sensor_values[11];
uint8_t temp_sensor_values[11];
uint8_t line_weight = 127;
uint8_t previous_pickup_station = No;
uint8_t tape_reference = 150;

uint16_t pickup_iterator = 0;


uint8_t not_on_tape()	{
	uint8_t not_on_tape = 1;
	for(uint8_t i = 0; i<=10;i++)	{
		if(get_sensor_surface(i)==1)	{
			not_on_tape = 0;
		}
	}
	return not_on_tape;
}

uint16_t set_tape_reference(uint8_t id, uint16_t input_tape_reference)	{
	tape_reference = input_tape_reference;
	return 0;
}


uint16_t return_linesensor(uint8_t id, uint16_t sensor_pair)	{
	if (sensor_pair != 5)
	return ((uint16_t)sensor_values[2*sensor_pair + 1] << 8) | (uint16_t) sensor_values[2*sensor_pair];
	else
	return (uint16_t) sensor_values[2*sensor_pair];
}

//Formats the output to accomodate the chassi and transmits it on the bus
uint16_t return_line_weight(uint8_t id, uint16_t metadata)	{
	composite_output_type chassi_output = 1;
	if(pickup_station == Left)
		chassi_output = station_Left;
	else if(pickup_station == No)
		chassi_output = station_No;
	else if(pickup_station == Right)
		chassi_output = station_Right;
	//if(not_on_tape())
		//chassi_output = No_tape;
	return (((uint16_t)(chassi_output) << 8) | line_weight);
}

void line_init(){
	linesensor_channel = 0;
	ADCSRA = 0b10001111;
	DDRD = 0b11111111;
	DDRB = 0b11111111;
	ADMUX = (1 << ADLAR);
	
	PORTB = linesensor_channel;
	ADCSRA |= (1 << ADSC);
}
void update_linesensor_values() {
	temp_sensor_values[linesensor_channel] = ADCH;
	
	if (linesensor_channel == 10) {
		linesensor_channel = 0;
		for(uint8_t i = 0; i<=10;i++)	{
				sensor_values[i]=temp_sensor_values[i];
		}
	}
	else {
		linesensor_channel = linesensor_channel + 1;
	}
	PORTB = linesensor_channel;
			
	ADCSRA |= (1 << ADSC);
	
}
station_type get_sensor_surface(uint8_t sensor_id)	{
	if(sensor_values[sensor_id] >= tape_reference)
		return Tape;
	else
		return Floor;
}
void calculate_line_weight()	{
	cli();
	uint32_t temp_line_weight;
	uint16_t tot_weight;
	uint16_t sensor_scale;
	uint8_t current_sensor;
	current_sensor = 0;
	tot_weight = 0;
	sensor_scale = 11;
	temp_line_weight = 0;
	while(current_sensor<=10)	{
		tot_weight = tot_weight + sensor_values[current_sensor];
		temp_line_weight = temp_line_weight + sensor_values[current_sensor] * sensor_scale;
		current_sensor++;
		if(current_sensor <= 10)
			sensor_scale = sensor_scale + 256/11;
	}
	line_weight = temp_line_weight / tot_weight;
	sei();
}

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


void pickup_station_detection() {
	cli();
	if (previous_pickup_station == Left && is_tape_right()) {
		previous_pickup_station = No;
	}
	else if (previous_pickup_station == Right && is_tape_left()) {
		previous_pickup_station = No;
	}
	
	else if (previous_pickup_station == Left && !is_tape_left() && pickup_iterator > 1950) {
		previous_pickup_station = No;
		pickup_iterator = 0;
	}
	else if (previous_pickup_station == Right && !is_tape_right() && pickup_iterator > 1950) {
		previous_pickup_station = No;
		pickup_iterator = 0;
	}
	else if (pickup_iterator > 0) {
		--pickup_iterator;
	}
	
	else if (previous_pickup_station == No) {
		if (is_tape_right()) {
			previous_pickup_station = Right;
			pickup_iterator = 2000;
		}
		else if (is_tape_left()) {
			previous_pickup_station = Left;
			pickup_iterator = 2000;
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


void update_linesensor()	{
	update_linesensor_values();
	calculate_line_weight();
	pickup_station_detection();
}
void init_linesensor_calibration()	{
	//setup ADC for calibration-routine
	ADCSRA = 0b10000111;
	ADCSRA |= (1 << ADEN);
	ADMUX = 0b00100000;
	DDRB = 0b11111111;
}

void calculate_average(uint8_t sensor_references[11][10], uint8_t average[11]) {
	
	for(uint8_t i = 0; i<=10;i++)	{
		for(uint8_t j = 0; j<=9;j++)	{
			average[i] = average[i] + sensor_references[i][j];
			if(j==9)	{
				average[i] = average[i]/10;
			}
		}
	}
}

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

void calibrate_linesensor(uint8_t id, uint16_t calibration_variable)	{	
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
	bus_transmit(BUS_ADDRESS_COMMUNICATION,10,(uint16_t)tape_reference);
	line_init();
	sei();
}


