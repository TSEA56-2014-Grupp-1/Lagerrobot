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
uint16_t adtest = 0;

ISR(ADC_vect) {
	switch (ADMUX & 0b00011111) {
		case 0 :
			update_linesensor();
		case 1 :
			//update_distance_sensor_1(); this function should be in the "arm-sensor code"
		break;
		case 2 : 
			update_distance_sensor_2();
		break;
		case 3 :
			update_distance_sensor_3();
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
	
    while(1)
    {
		adtest = ad_interpolate(263,2);
		adtest = ad_interpolate(400,2);
		adtest = ad_interpolate(300,2);
		adtest = ad_interpolate(200,2);
		adtest = ad_interpolate(100,2);
		adtest = ad_interpolate(50,2);
        //TODO:: Please write your application code 
    }
}