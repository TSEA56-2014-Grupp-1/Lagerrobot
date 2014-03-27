/*
 * linesensor.c
 *
 * Created: 2014-03-27 09:08:16
 *  Author: Karl
 */ 

#include <avr/io.h>
#include "linesensor.h"

//uint8_t line_weight;
uint8_t sensor_channel;
uint8_t sensor_values[11];
uint16_t line_weight;
uint16_t temp_line_weight;
uint8_t current_sensor;
uint16_t tot_weight;
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
		line_weight = calculate_line_weight();
	}
	else {
		sensor_channel = sensor_channel + 1;
	}
	PORTB = sensor_channel;
			
	ADCSRA |= (1 << ADSC);
	
}

uint8_t calculate_line_weight()	{
	//temp_line_weight = 0;
	line_weight = 0;
	tot_weight = 0;
	current_sensor = 0;
// 	for (current_sensor = 0; current_sensor<=9;current_sensor++)	{
// 		if(sensor_values[current_sensor] > sensor_values[current_sensor + 1])	{
// 			temp_line_weight = current_sensor;
// 		}
// 	}
	
	while(current_sensor <= 10)	{
		if (sensor_values[current_sensor]> sensor_values[current_sensor + 1])	{
			temp_line_weight = current_sensor;
		}
		current_sensor++;
	}
	//temp_line_weight = temp_line_weight/tot_weight;
	return temp_line_weight;
}
