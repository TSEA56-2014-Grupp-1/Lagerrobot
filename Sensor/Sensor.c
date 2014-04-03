/*
 * Sensor.c
 *
 * Created: 2014-03-27 08:55:21
 *  Author: Karl
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "linesensor.h"
#include "sidescanner.h"
#include "../shared/bus.h"
#include "sidescanner.h"

uint8_t n;


ISR(ADC_vect) {
	switch (ADMUX & 0b00011111) {
		case 0 :
			update_linesensor();
		case 1 :
			update_distance();
		break;
		case 2 : 
		
		break;
		case 3 :
		
		break;
	}
}

int main(void)
{
	bus_init(4);
	bus_register_response(4, return_line_weight);
	//sidescanner
	sidescanner_init();
	scanner_left_position(0);
	wait_scanner_servo(1000);
	
	//calibrate_linesensor();
	//line_init();
	sidescanner_init();
	sei();
	
    while(1)
    {
		scanner_left_position(n);
		scanner_right_position(n);
		n = n +10;
        //TODO:: Please write your application code 
    }
}