/*
 * Sensor.c
 *
 * Created: 2014-03-27 08:55:21
 *  Author: Karl & Philip
 */ 

#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "linesensor.h"
#include "../shared/bus.h"
#include "../RFID_scanner/RFID_scanner.h"
#include "../shared/usart.h"
#include "sidescanner.h"
#include "distance_sensors.h"

uint8_t sensor_task = 0;

ISR(ADC_vect) {
	if ((ADMUX & 0b00011111) == 0) {
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
			object_detection(sensor_left);
			sensor_task = 3;
			break;
		case 2:
			object_detection(sensor_right);
			sensor_task = 3;
			break;
		default:
			_delay_ms(10);
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
		sidescanner_init(sensor_left);
	}
	else if (data == 2) {
		sidescanner_init(sensor_right);
	}
}

int main(void)
{
	bus_init(BUS_ADDRESS_SENSOR);
	usart_init(520);
	RFID_scanner_init();
	
	bus_register_receive(2, calibrate_linesensor);
	bus_register_response(3, return_linesensor);
	bus_register_response(4, return_line_weight);
	bus_register_response(5, set_tape_reference);
	bus_register_receive(7, RFID_disable_reading);	
	bus_register_receive(8, RFID_enable_reading);	
	bus_register_receive(9, set_task);
	bus_register_receive(10, read_rfid);
	bus_register_receive(11, send_line_data);

	sei();
    while(1)
    {
		sensor_task_manager();
    }
}