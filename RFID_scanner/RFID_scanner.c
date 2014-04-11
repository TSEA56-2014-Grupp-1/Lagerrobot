/*
 * GccApplication3.c
 *
 * Created: 2014-04-03 11:13:39
 *  Author: Erik 
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#include <util/delay.h>
#include "../RFID_scanner/RFID_scanner.h"
#include "../shared/usart.h"
#include "../shared/bus.h"
#include "../shared/LCD_interface.h"

uint8_t check_error = 0;
uint8_t station_RFID[12];
uint8_t carrying_RFID[12];
uint8_t tag = 15;


const uint8_t  RFID_B80[] = {
	0x0A, 0x32, 0x36, 0x30, 0x30, 0x44, 0x42, 0x39, 0x31, 0x36, 0x41, 0x0D
};
const uint8_t RFID_B81[] = {
	0x0A, 0x32, 0x36, 0x30, 0x30, 0x44, 0x42, 0x32, 0x37, 0x43, 0x34, 0x0D
};
const	uint8_t RFID_B82[] = {
	0x0A, 0x32, 0x36, 0x30, 0x30, 0x44, 0x38, 0x41, 0x44, 0x30, 0x45, 0x0D
};
const	uint8_t RFID_B83[] = {
	0x0A, 0x32, 0x36, 0x30, 0x30, 0x44, 0x38, 0x41, 0x42, 0x38, 0x42, 0x0D
};
const uint8_t RFID_B84[] = {
	0x0A, 0x32, 0x36, 0x30, 0x30, 0x44, 0x33, 0x44, 0x38, 0x32 ,0x46, 0x0D
};
const uint8_t RFID_B85[] = {
	0x0A, 0x32, 0x36, 0x30, 0x30, 0x44 ,0x33, 0x44, 0x42, 0x42, 0x38, 0x0D
};

uint16_t return_rfid_tag(uint8_t id, uint16_t metadata)
{
	return (uint16_t) read_RFID();
}

void RFID_scanner_init()
{
	DDRD = 0b00000100; // Set PD2 as Output
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
			display(0, "81");
	}
	else if (station == 82)
	{
			display(1,"station");
			display(0, "82");
	}
	else if (station == 83)
	{
			display(1,"station");
			display(0, "83");
	}
	else if (station == 84)
	{
			display(1,"station");
			display(0, "84");
	}
	else if (station == 85)
	{
			display(1,"station");
			display(0, "85");
	}
	else 
	{
			display(1,"station");
			display(0, "unkown");
	}
}


void RFID_read_usart()
{
	uint8_t i = 0;
	for (i = 0; i <= 11; ++i) // Read 12 bytes
	{
		check_error = usart_read_byte(&station_RFID[i]);
		if (check_error == 1)
		break;
	}
}

uint8_t read_RFID()
{
	PORTD = (0 << PORTD2); // Enable reading
	uint8_t i;
	for (i = 0; i < 70; ++i)
	{
		RFID_read_usart();
		if(identify_station_RFID() != 15)
		{
		PORTD |= (1 << PORTD2); // Disable reading
		return identify_station_RFID();
		}
	}
	PORTD |= (1 << PORTD2); // Disable reading
	return 15;
}


uint8_t compare_RFID_arrays(uint8_t station_RFID[12], const uint8_t current_compare_RFID[12])
{
	uint8_t i;
	for (i = 1; i <= 11; ++i)
	{
		if(station_RFID[i] != current_compare_RFID[i])
		{
			return 0;
		}
	}
	return 1;
}

uint8_t identify_station_RFID()
{
	if (compare_RFID_arrays(station_RFID, RFID_B80))
	return 0;
	else if (compare_RFID_arrays(station_RFID, RFID_B81))
	return 1;
	else if (compare_RFID_arrays(station_RFID, RFID_B82))
	return 2;
	else if (compare_RFID_arrays(station_RFID, RFID_B83))
	return 3;
	else if (compare_RFID_arrays(station_RFID, RFID_B84))
	return 4;
	else if (compare_RFID_arrays(station_RFID, RFID_B85))
	return 5;
	else
	return 15;
};
