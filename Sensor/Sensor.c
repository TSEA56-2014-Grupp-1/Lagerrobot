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
#include "../shared/LCD_interface.h"

ISR(ADC_vect) {
	switch (ADMUX & 0b00011111) {
		case 0 :
			update_linesensor();

		//XXX: These wont be needed anymore
		case 1 :
			//update_distance_sensor_2();	//left sensor (adc1)
		break;
		case 2 :
			//update_distance_sensor_3(); //right sensor (adc2)
		break;
		case 3 :
			//update_distance_sensor_1(); //this function should be in the "arm-sensor code" (adc3)
		break;
	}
}


int main(void)
{

	bus_init(4);
	sidescanner_init(sensor_left);

	sei();

	object_detection(0, sensor_left);
// 	_delay_ms(4000);
    while(1)
    {
//     	scanner_set_position(0, sensor_left);
// 	 	_delay_ms(4000);
// 	 	scanner_set_position(90, sensor_left);
// 	 	_delay_ms(4000);
// 	 	scanner_set_position(180, sensor_left);
//     	_delay_ms(4000);
// 		ADCSRA |= (1 << ADSC);
// 		while (ADCSRA & (1 << ADSC));
// 		ADCSRA &= ~(1 << ADIF);
// 		display(1,"ADC: %d",ADC);
// 		_delay_ms(400);
    }
}
