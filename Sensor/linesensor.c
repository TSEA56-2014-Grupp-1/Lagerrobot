/*
 * linesensor.c
 *
 * Created: 2014-03-27 09:08:16
 *  Author: Karl
 */ 

#include <avr/io.h>
#include "linesensor.h"

uint8_t sensor_channel;
uint8_t sensor_values[11];

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
