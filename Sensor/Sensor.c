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

void sensor_task_manager()	{
	
	switch (sensor_task)	{
		case 0:
		calculate_line_weight();
		break;
		case 1:
		//XXX: object_detection(sensor_left);
		break;
		case 2:
		//XXX: object_detection(sensor_right);
		break;
	}
}

void set_task(uint8_t id, uint16_t data)	{
	sensor_task = data;
	if (data == 0) {
		clear_pickupstation();
		line_init();
	}
	else if (data == 1) {
		//XXX: sidescanner_init(sensor_left);
	}
	else if (data == 2) {
		//XXX: sidescanner_init(sensor_right);
	}
}

int main(void)
{
	bus_init(BUS_ADDRESS_SENSOR);
	bus_register_response(4, return_line_weight);
	bus_register_response(3, return_linesensor);
	bus_register_response(5, set_tape_reference);
	bus_register_receive(2, calibrate_linesensor);
	bus_register_receive(9, set_task);
	
	RFID_scanner_init();
	usart_init(520);
	sei();
    while(1)
    {
		sensor_task_manager();
    }
}