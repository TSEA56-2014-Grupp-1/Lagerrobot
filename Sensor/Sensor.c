/*
 * Sensor.c
 *
 * Created: 2014-03-27 08:55:21
 *  Author: Karl
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "linesensor.h"

ISR(ADC_vect) {
	switch (ADMUX & 0b00011111) {
		case 0 :
			update_linesensor_values();
		case 1 :
		
		break;
		case 2 : 
		
		break;
		case 3 :
		
		break;
	}
}

int main(void)
{
	line_init();
	
	sei();
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}