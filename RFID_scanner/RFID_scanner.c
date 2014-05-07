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

uint8_t station_RFID[12];

//-----RFID_tags-----
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

void RFID_disable_reading(uint8_t id, uint16_t metadata)
{
	PORTD |= (1 << PORTD2); // Disable reading
}

void RFID_enable_reading(uint8_t id, uint16_t metadata)
{
	PORTD = PORTD & (0 << PORTD2); // Enable reading
}

void RFID_scanner_init()
{
	DDRD = 0b00000100; // Set PD2 as Output
	//PORTD = PORTD & (0 << PORTD2); // Disable reading
}

uint8_t RFID_read_usart()
{
	uint8_t i = 0;
	for (i = 0; i <= 11; ++i) // Read 12 bytes
	{
		//if (usart_read_byte(&station_RFID[i]) == 1)
		//return 1;
		usart_read_byte(&station_RFID[i]);
	}
	return 0;
}

uint16_t read_RFID(uint8_t id, uint16_t metadata)
{	
	uint8_t i = 0;
	for(i= 0; i <= 150; ++i)
	{
		RFID_read_usart();
		if(station_RFID[0] == 0x0A) // Correct startbyte found
		{
			PORTD |= (1 << PORTD2); // Disable rfid reading
			return identify_station_RFID();
		}
	}
	PORTD |= (1 << PORTD2); // Disable rfid reading
	return station_RFID[0]; //  time out or wrong startbyte found
}


uint8_t compare_RFID_arrays(const uint8_t current_compare_RFID[12])
{
	uint8_t i;
	for (i = 7; i <= 10; ++i)
	{
		if(station_RFID[i] != current_compare_RFID[i])
			return 0;
	}
	return 1;
}

uint8_t identify_station_RFID()
{
	if (compare_RFID_arrays(RFID_B80))
	return 80;
	else if (compare_RFID_arrays(RFID_B81))
	return 81;
	else if (compare_RFID_arrays(RFID_B82))
	return 82;
	else if (compare_RFID_arrays(RFID_B83))
	return 83;
	else if (compare_RFID_arrays(RFID_B84))
	return 84;
	else if (compare_RFID_arrays(RFID_B85))
	return 85;
	else
	return 1; // no match
};
