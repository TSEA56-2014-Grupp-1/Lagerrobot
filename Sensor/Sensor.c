/*
 * Sensor.c
 *
 * Created: 2014-03-27 08:55:21
 *  Author: Karl & Philip
 */

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "linesensor.h"
#include "../shared/bus.h"
#include "../RFID_scanner/RFID_scanner.h"
#include "../shared/LCD_interface.h"
#include "../shared/usart.h"
#include "sidescanner.h"
#include "distance_sensors.h"

uint8_t sensor_task = 5;

ISR(ADC_vect) {
	if ((ADMUX & 0b00011111) == 0) {
 			update_linesensor_values();
			pickup_station_detection();
	}
}

void sensor_task_manager()	{
	uint8_t status;
	switch (sensor_task)	{
		case 0:
			calculate_line_weight();
			break;
		case 1:
			object_detection(sensor_left);
			sensor_task = 4;
			break;
		case 2:
			object_detection(sensor_right);
			sensor_task = 4;
			break;
		case 3:
			ADCSRA &= ~(1 << ADIE); // disable ADC-interrupt
			clear_station_RFID();

			status = RFID_read_usart();
			if (status) {
				display(0, "no id %u", status);
				send_rfid(0);
			}
			else {
				uint8_t id = identify_station_RFID();
				display(0, "yes id %u", status);
				display(1, "id: %u", id);
				send_rfid(identify_station_RFID());
			}

			sensor_task = 4;
		default:
			_delay_ms(10);
			break;
	}
}

void set_task(uint8_t id, uint16_t data)	{
	sensor_task = (uint8_t)data;
	if (sensor_task == 0) {
		clear_pickupstation();
		line_init();
	}
	else if (sensor_task == 1) {
		sidescanner_init(sensor_left);
	}
	else if (sensor_task == 2) {
		sidescanner_init(sensor_right);
	}
}

void read_rfid(uint8_t id, uint16_t metadata)
{
	sensor_task = 3;
}

int main(void)
{
	bus_init(BUS_ADDRESS_SENSOR);
	usart_init(520);
	RFID_scanner_init();
 	sidescanner_init(sensor_left);
 	sidescanner_init(sensor_right);

//	line_init();

	bus_register_receive(2, calibrate_linesensor);
	bus_register_response(3, return_linesensor);
	bus_register_response(4, return_line_weight);
	bus_register_response(5, set_tape_reference);
	bus_register_receive(7, RFID_disable_reading);
	bus_register_receive(8, RFID_enable_reading);
	bus_register_receive(9, set_task);
	bus_register_receive(10, read_rfid);
	bus_register_response(11, return_line_weight);

// 	scanner_set_position(180,sensor_right);
// 	_delay_ms(6000);

	sei();
    while(1)
    {
		sensor_task_manager();
    }
}
