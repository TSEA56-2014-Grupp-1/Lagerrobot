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

//Used for calibration
uint8_t tape_sensor_references[11][10];
uint8_t floor_sensor_references[11][10];
uint16_t tape_average[11];
uint16_t floor_average[11];
uint16_t total_tape_average;
uint16_t total_floor_average;

//Defining different datatypes	
typedef uint8_t surface_type;
enum {Floor, tape};

typedef uint8_t station_type;
enum station_type {Left, No, Right};

typedef uint8_t line_type;
enum line_type {line, interrupt, crossing};

typedef uint8_t composite_output_type;
enum composite_output_type {station_Left, station_No, station_Right,No_tape};

//Declaring variables with homemade datatypes
composite_output_type chassi_output;
line_type line_status = 1;
surface_type sensor_surface_types[11] = {0,0,0,0,0,0,0,0,0,0,0};	
station_type pickup_station = No;


uint8_t sensor_channel;
uint8_t sensor_values[11];
uint8_t temp_sensor_values[11];
uint8_t line_weight = 127;
uint8_t ad_value = 0;
double sensor_scale[11];

uint16_t pickup_iterator = 0;
uint8_t previous_pickup_station = No;

uint8_t tape_reference = 150;


uint8_t not_on_tape()	{
	uint8_t not_on_tape = 1;
	for(uint8_t i = 0; i<=10;i++)	{
		if(sensor_surface_types[i]==1)	{
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
	return ((uint16_t)sensor_values[2*sensor_pair] << 8) | (uint16_t) sensor_values[2*sensor_pair + 1];
	else
	return (uint16_t) sensor_values[2*sensor_pair];
}

//Formats the output to accomodate the chassi and transmits it on the bus
uint16_t return_line_weight(uint8_t id, uint16_t metadata)	{
	chassi_output = 1;
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
	sensor_channel = 0;
	ADCSRA = 0b10001111;
	DDRD = 0b11111111;
	DDRB = 0b11111111;
	ADMUX = (1 << ADLAR);
	
	PORTB = sensor_channel;
	ADCSRA |= (1 << ADSC);
}
void update_linesensor_values() {
	temp_sensor_values[sensor_channel] = ADCH;
	
	if (sensor_channel == 10) {
		sensor_channel = 0;
		for(uint8_t i = 0; i<=10;i++)	{
//			if(sensor_scale<temp_sensor_values[i])	{
				sensor_values[i]=temp_sensor_values[i] - sensor_scale[i];
// 			}
// 			else
// 			{
// 				sensor_values[i] = temp_sensor_values;//sensor_values[i] = sensor_scale - temp_sensor_values[i];
//  			}
		}
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
			sensor_surface_types[current_sensor] = Floor;
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

uint8_t get_tape_width()	{
	uint8_t tape_width;
	uint8_t current_sensor;
	tape_width = 0;
	current_sensor = 0;
	
	while(current_sensor <=10)	{
		tape_width = tape_width + sensor_surface_types[current_sensor];
		current_sensor++;
	}
	return tape_width;
}
uint8_t is_tape_right()	{
	uint8_t number_of_tape_sensors = 0;
	for(uint8_t i = 7; i<=10; i++)	{
		if (sensor_surface_types[i] == 1)
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
		if (sensor_surface_types[i] == 1)
		number_of_tape_sensors++;
	}
	if(number_of_tape_sensors >= 4)
		return 1;
	else
		return 0;
}


void pickup_station_detection() {
	cli();
	if (previous_pickup_station == No) {
		if (is_tape_right()) {
			previous_pickup_station = Right;
			pickup_iterator = 2000;
		}
		else if (is_tape_left()) {
			previous_pickup_station = Left;
			pickup_iterator = 2000;
		}
	}
	else if (previous_pickup_station == Left && pickup_iterator == 0) {
		pickup_station = Left;	
	}
	else if (previous_pickup_station == Right && pickup_iterator == 0) {
		pickup_station = Right;
	}
	else if (previous_pickup_station == Left && is_tape_right()) {
		previous_pickup_station = No;
	}
	else if (previous_pickup_station == Right && is_tape_left()) {
		previous_pickup_station = No;
	}
	else if (pickup_iterator > 0)
		--pickup_iterator;
	sei();
}


void line_break_detection()	{	
	cli();
	uint8_t current_sensor;
	uint8_t total_tape;
	total_tape = 0;
	current_sensor = 0;
	line_status = interrupt;
	while(current_sensor <= 10)	{
		if(sensor_surface_types[current_sensor] == tape)	{
			line_status = line;
			total_tape++;
		}
		current_sensor++;
	}
	if(total_tape == 11)
		line_status = crossing;
	sei();
	};
void update_linesensor()	{
	update_linesensor_values();
	update_linesensor_surfaces();
	calculate_line_weight();
	pickup_station_detection();
	line_break_detection();

}
void init_linesensor_calibration()	{
	cli();
	//setup ADC for calibration-routine
	ADCSRA = 0b10000111;
	ADCSRA |= (1 << ADEN);
	ADMUX = 0b00100000;
	DDRB = 0b11111111;
	total_tape_average = 0;
	total_floor_average = 0;
	sei();
}
//should set sensor_scale-values and tape_reference
uint16_t calibrate_linesensor(uint8_t id, uint16_t calibration_variable)	{	
	cli();	
	uint8_t sensor_scale_temp_floor;
	uint8_t sensor_scale_temp_tape;
	init_linesensor_calibration();
	if(calibration_variable == 1)
		calibrate_tape();
	else if (calibration_variable == 0)
		calibrate_floor();

	//code for calculating sensor_scale-needs testing!
	if((total_floor_average != 0) && (total_tape_average != 0))	{
		for(uint8_t i = 0; i<=10; i++)	{
			if(tape_average[i] < total_tape_average)
				sensor_scale_temp_tape = total_tape_average - tape_average[i];
			else
				sensor_scale_temp_tape = 0;//tape_average[i] - total_tape_average;
			if(floor_average[i] < total_floor_average)
				sensor_scale_temp_floor = total_floor_average - floor_average[i];
			else
				sensor_scale_temp_floor = 0;//floor_average[i] - total_floor_average;
			
			sensor_scale[i] = (sensor_scale_temp_floor + sensor_scale_temp_tape)/2;	
	}
	//calculate tape_reference
	tape_reference = (total_floor_average + total_tape_average)/2;
	}
	line_init();
	sei();
	bus_transmit(BUS_ADDRESS_COMMUNICATION,10,tape_reference);
	return 0;
}

void calibrate_tape()	{

for(uint8_t j = 0; j <= 9; j++)	{	
		for(uint8_t i = 0; i <= 10; i++)		{
			PORTB = i;
			ADCSRA |= 0b01000000;
			while (ADCSRA &(1<<ADSC)){}
			tape_sensor_references[i][j] = ADCH;
		}	
	}
	
	for(uint8_t i = 0; i<=10;i++)	{
		for(uint8_t j = 0; j<=9;j++)	{
			tape_average[i] = tape_average[i] + tape_sensor_references[i][j];
			if(j==9)	{
				tape_average[i] = tape_average[i]/10;
			}
		}
	}
	
	for(uint8_t i = 0; i<=10;i++)	{
		total_tape_average = total_tape_average + tape_average[i];
		line_weight = total_tape_average;
	}
	total_tape_average = total_tape_average/11;
}

void calibrate_floor()	{
	for(uint8_t j = 0; j <= 9; j++)	{	
		for(uint8_t i = 0; i <= 10; i++)		{
			PORTB = i;
			ADCSRA |= 0b01000000;
			while(ADCSRA &(1<<ADSC)){}
			floor_sensor_references[i][j] = ADCH;
		}
	}
	for(uint8_t i = 0; i<=10;i++)	{
		for(uint8_t j = 0; j<=9;j++)	{
			floor_average[i] = floor_average[i] + floor_sensor_references[i][j];
			if(j==9)	{
				floor_average[i] = floor_average[i]/10;
			}
		}
	}
	for(uint8_t i = 0; i<=10; i++)	{
		total_floor_average = total_floor_average + floor_average[i];
		if(i == 10)
			total_floor_average = total_floor_average/11;
	}
}

