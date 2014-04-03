/*
 * linesensor.c
 *
 * Created: 2014-03-27 09:08:16
 *  Author: Karl
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "linesensor.h"

// #define F_CPU 20000000UL;
// #include <util/delay.h>
//ska flyttas in i calibrate
	uint8_t tape_sensor_references[11][10];
	uint8_t floor_sensor_references[11][10];
	uint16_t tape_average[11];
	uint16_t floor_average[11];
	uint16_t total_tape_average;
	uint16_t total_floor_average;

typedef uint8_t surface_type;
enum {floor, tape};

typedef uint8_t station_type;
enum station_type {Left, No, Right};

typedef uint8_t line_type;
enum line_type {line, interrupt, crossing};

//Hemmagjorda datatyper
line_type line_status;
surface_type sensor_surface_types[11];	
station_type pickup_station;

//Heltal
uint8_t sensor_channel;
uint8_t sensor_values[11];
uint8_t temp_sensor_values[11];
uint8_t line_weight;
double sensor_scale[11];
// for (uint8_t i = 0; i <= 10 ;i++)	{
// 	sensor_scale[i] = 1;
// }

uint8_t tape_reference = 100;


uint16_t return_line_weight(uint8_t id, uint16_t metadata)	{
	return (uint16_t)line_weight;
}

void line_init(){
	sensor_channel = 0;
	ADCSRA = 0b10001111;
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
	
	if((tape_width > 4 )&& (line_weight < 127) && (line_status == 0))
		pickup_station = Left;
	else if ((tape_width > 4) && (line_weight > 127) && (line_status == 0))
		pickup_station = Right;
	else
		pickup_station = No;
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

void calibrate_linesensor()	{	//should set sensor_scale-values and tape_reference
	cli();	
	uint8_t sensor_scale_temp_floor;
	uint8_t sensor_scale_temp_tape;
	//setup ADC for calibration-routine
	ADCSRA = 0b10000111;
	ADCSRA |= (1 << ADEN);
	ADMUX = 0b00100000;
	DDRB = 0b11111111;
	calibrate_tape();
	
	calibrate_floor();
	
	//code for calculating sensor_scale-needs testing!
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
	sei();
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