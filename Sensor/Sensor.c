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
//#include "../shared/LCD_interface.h"
#include "../RFID_scanner/RFID_scanner.h"
#include "../shared/usart.h"

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
	usart_init(520);
	RFID_scanner_init();
	bus_init(BUS_ADDRESS_SENSOR);
	bus_register_response(4, return_line_weight);
	bus_register_response(3, return_linesensor);
	bus_register_response(5, set_tape_reference);
	bus_register_response(2, calibrate_linesensor);
	bus_register_response(6, read_RFID);
	bus_register_receive(7, RFID_disable_reading);
	bus_register_receive(8, RFID_enable_reading);
	//lcd_interface_init();
	//display(0, "Hej");
	//calibrate_linesensor();
	line_init();
	//init_linesensor_calibration();
	sei();
	//bus_transmit(5,4,5);
    while(1)
    {
        //TODO:: Please write your application code 
    }
}