/*
 * linesensor.c
 *
 * Created: 2014-03-27 09:08:16
 *  Author: Karl
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "linesensor.h"

typedef uint8_t surface_type;
enum {floor, tape};

typedef uint8_t station_type;
enum station_type {Left, No, Right};


//Hemmagjorda datatyper
surface_type line_break;
surface_type sensor_surface_types[11];	
station_type pickup_station;

//Heltal
uint8_t sensor_channel;
uint8_t sensor_values[11];
uint8_t line_weight;

uint8_t tape_reference = 100;


void line_init(){
	sensor_channel = 0;
	ADCSRA = 0b10001111;
	DDRD = 0b11111111;
	DDRB = 0b11111111;
	ADMUX = (1 << ADLAR);
	
	PORTB = sensor_channel;
	ADCSRA |= (1 << ADSC);
}

void update_linesensor_values() {
	sensor_values[sensor_channel] = ADCH;
	
	if (sensor_channel == 10) {
		sensor_channel = 0;
	}
	else {
		sensor_channel = sensor_channel + 1;
	}
	PORTB = sensor_channel;
			
	ADCSRA |= (1 << ADSC);
	
}
void update_linesensor_surfaces()	{
	uint8_t current_sensor;
	current_sensor = 0;
	while(current_sensor <=10)	{
		if(sensor_values[current_sensor] >= tape_reference)
			sensor_surface_types[current_sensor] = tape;
		else
			sensor_surface_types[current_sensor] = floor;
		current_sensor++;
	}
	
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
void pickup_station_detection() {
	uint8_t current_sensor;
	uint8_t tape_width;

	tape_width =0;
	current_sensor = 0;

	while(current_sensor <=10)	{
		tape_width = tape_width + sensor_surface_types[current_sensor];
		current_sensor++;
	}
	
	if((tape_width > 4 )&& (line_weight < 127))
		pickup_station = Left;
	else if ((tape_width > 4) && (line_weight > 127))
		pickup_station = Right;
	else
		pickup_station = No;
}
void line_break_detection()	{	//better name for line_break?
	cli();
	uint8_t current_sensor;
	current_sensor = 0;
	line_break = floor;
	while(current_sensor <= 10)	{
		if(sensor_surface_types[current_sensor] == tape)
			line_break = tape;
		current_sensor++;
	}
	sei();
	};

void update_linesensor()	{
	update_linesensor_values();
	update_linesensor_surfaces();
	calculate_line_weight();
	pickup_station_detection();
	line_break_detection();
}