/*
 * GccApplication3.c
 *
 * Created: 2014-04-03 11:13:39
 *  Author: Erik 
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "RFID_scanner.h"
#include "../shared/usart.h"
#include "../shared/bus.h"
#include "../shared/LCD_interface.h"


uint8_t station_RFID[12];
uint8_t carrying_RFID[12];

void RFID_scanner_init()
{
	DDRD = 0b0000100; // Set PD2 as Output
}

void station_to_LCD(uint8_t station){
	if (station == 80)
	{
	display(1,"station");
	display(0, "80");
	}
	else if (station == 81)
	{
			display(1,"station");
			display(0, "80");
	}
	else if (station == 82)
	{
			display(1,"station");
			display(0, "80");
	}
	else if (station == 83)
	{
			display(1,"station");
			display(0, "80");
	}
	else if (station == 84)
	{
			display(1,"station");
			display(0, "80");
	}
	else if (station == 85)
	{
			display(1,"station");
			display(0, "80");
	}
	else 
	{
			display(1,"station");
			display(0, "80");
	}
}


void read_RFID()
{
	PORTD = (0 << PORTD2); // Enable reading
	uint8_t i = 0;
	for (i = 0; i <= 11; ++i) // Read 12 bytes
	{
		//usart_read_byte(&station_RFID[i]);
	}
	PORTD = PORTD & (1 << PORTD2); // Disable reading
}


uint8_t compare_RFID_arrays(uint8_t station_RFID[12], const uint8_t current_compare_RFID[12])
{
	uint8_t i;
	for (i = 0; i <=11; ++i)
	{
		if(current_compare_RFID[i] != current_compare_RFID[i])
		return 0;
	}
	return 1;
}

uint8_t identify_station_RFID()
{
	if (compare_RFID_arrays(station_RFID, RFID_B80))
	return 80;
	else if (compare_RFID_arrays(station_RFID, RFID_B81))
	return 81;
	else if (compare_RFID_arrays(station_RFID, RFID_B82))
	return 82;
	else if (compare_RFID_arrays(station_RFID, RFID_B83))
	return 83;
	else if (compare_RFID_arrays(station_RFID, RFID_B84))
	return 84;
	else if (compare_RFID_arrays(station_RFID, RFID_B85))
	return 85;
	else
	return 0;
};


int main(void)
{
	RFID_scanner_init();
	usart_init(520);
	bus_init(4); //Bus address sensor
	lcd_interface_init();
	sei();
	read_RFID();
	uint8_t station = identify_station_RFID();
	station_to_LCD(station);
    while(1)
    {
        //TODO:: Please write your application code 
    }
}
