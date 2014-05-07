/*
 * Sensor.c
 *
 * Created: 2014-03-27 08:55:21
 *  Author: Karl & Philip
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "linesensor.h"
#include "../shared/bus.h"

ISR(ADC_vect) {
	switch (ADMUX & 0b00011111) {
		case 0 :
 			update_linesensor_values();
			pickup_station_detection();
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
	bus_init(BUS_ADDRESS_SENSOR);
	bus_register_response(4, return_line_weight);
	bus_register_response(3, return_linesensor);
	bus_register_response(5, set_tape_reference);
	bus_register_receive(2, calibrate_linesensor);
	bus_register_receive(9, clear_pickupstation);

	line_init();
	sei();

    while(1)
    {
		calculate_line_weight();
    }
}