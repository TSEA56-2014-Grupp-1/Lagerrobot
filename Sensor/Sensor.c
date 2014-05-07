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
#include "distance_sensors.h"

ISR(ADC_vect) {
	switch (ADMUX & 0b00011111) {
		case 0 :
			update_linesensor();
		case 1 :
			update_distance_sensor_2();	//left sensor (adc1)
		break;
		case 2 : 
			update_distance_sensor_3(); //right sensor (adc2)
		break;
		case 3 :
			//update_distance_sensor_1(); //this function should be in the "arm-sensor code" (adc3)
		break;
	}
}


int main(void)
{

	bus_init(4);
	bus_register_response(4, return_line_weight);
	//sidescanner
	sidescanner_init();
	
	//calibrate_linesensor();
	//line_init();
	sei();

	left_object_detection(0, 0);

    while(1)
    {
		//sweep_left();
    }
}