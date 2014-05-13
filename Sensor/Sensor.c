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

uint8_t sensor_task = 0;

ISR(ADC_vect) {
	if ((ADMUX & 0b00011111 == 0)) {
 			update_linesensor_values();
			pickup_station_detection();
	}
}

int main(void)
{
	bus_init(BUS_ADDRESS_SENSOR);
	bus_register_response(4, return_line_weight);
	bus_register_response(3, return_linesensor);
	bus_register_response(5, set_tape_reference);
	bus_register_receive(2, calibrate_linesensor);
	bus_register_receive(9, set_task_follow_line);
	bus_register_receive(12, set_task_pickup_left);
	bus_register_receive(13, set_task_pickup_right);
	
	RFID_scanner_init();
	usart_init(520);
	sei();
    while(1)
    {
		sensor_task_manager();
    }
}

void sensor_task_manager()	{
	
	switch (sensor_task)	{
		case 0 :
			calculate_line_weight();
		case 1:
			//pickup station left
		case 2:
			//pickup station right
		}	
}

void set_task_pickup_left()	{
	sensor_task = 1;
	sidescanner_init();
}

void set_task_pickup_right()	{
	sensor_task = 2;
	sidescanner_init();
}

void set_task_follow_line()	{
	sensor_task = 0;
	clear_pickupstation();
	line_init();
}