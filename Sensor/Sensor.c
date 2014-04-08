/*
 * Sensor.c
 *
 * Created: 2014-03-27 08:55:21
 *  Author: Karl
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "linesensor.h"
#include "../shared/bus.h"
#include "../shared/LCD_interface.h"

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
	bus_init(BUS_ADDRESS_SENSOR);
	bus_register_response(4, return_line_weight);
	bus_register_response(3, return_linesensor);
	bus_register_response(5, set_tape_reference);
	bus_register_response(2, calibrate_linesensor);
	lcd_interface_init();
	
	//calibrate_linesensor();
	line_init();
	init_linesensor_calibration();
	sei();
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}