/*
 * linesensor.c
 *
 * Created: 2014-03-27 09:08:16
 *  Author: Karl
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "linesensor.h"

uint8_t sensor_channel;
uint8_t sensor_values[11];
uint8_t line_weight;


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
	temp_line_weight = temp_line_weight / tot_weight;
	sei();
	return temp_line_weight;
}
