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

ISR(ADC_vect) {
	switch (ADMUX & 0b00011111) {
		case 0 :
			update_linesensor();
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
	bus_init(4);
	bus_register_response(4, return_line_weight);
	
	//sidescanner
	sidescanner_init();
	scanner_left_position(0);
	wait_scanner_servo(1000);
	scan_left_side();
	
	//calibrate_linesensor();
	line_init();
	sei();
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}