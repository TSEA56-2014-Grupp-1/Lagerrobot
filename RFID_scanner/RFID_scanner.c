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
#include "../Sensor/linesensor.h"
#include "rfid_tags.h"

uint8_t station_RFID[12];


void RFID_disable_reading(uint8_t id, uint16_t metadata)
{
	PORTD |= (1 << PORTD2); // Disable reading
}

void RFID_enable_reading(uint8_t id, uint16_t metadata)
{
	PORTD &= ~(1 << PORTD2); // Enable reading
}

void RFID_scanner_init()
{
	DDRD = 0b00000100; // Set PD2 as Output
	PORTD |= (1 << PORTD2); // Disable reading
}

uint8_t RFID_read_usart()
{
	uint8_t i = 0;
	for (i = 0; i <= 11; ++i) // Read 12 bytes
	{
		if (usart_read_byte(&station_RFID[i]))
		return 1;
	}
	return 0;
}

void clear_station_RFID()
{
	uint8_t i = 0;
	for (i = 0; i <= 11; ++i)
	{
		station_RFID[i] = 0;
	}
}

void send_rfid(uint8_t station_tag)
{
	uint8_t station_data = get_station_data();
	uint16_t tag_and_station = ((uint16_t)station_data << 8) | (uint16_t)station_tag;
	uint16_t timeout_counter = 0;
		while(timeout_counter < 1000) {
			if (bus_transmit(BUS_ADDRESS_CHASSIS, 4, tag_and_station) == 0) {
				return;
			}
			++timeout_counter;	
		}
}

void read_rfid(uint8_t id, uint16_t metadata)
{
	ADCSRA &= ~(1 << ADIE); // disable ADC-interrupt
	uint8_t prev_station = identify_station_RFID();
	uint8_t current_station = 0;
	clear_station_RFID();
	uint16_t i = 0;
	for(i= 0; i <= 500; ++i) // XXX Increase this even more?
	{
		RFID_read_usart();
		if(station_RFID[11] == 0x0D) // Correct stopbyte found
		{
			current_station = identify_station_RFID();
			if (current_station != prev_station)
			{
				break; 
			}
		}
		_delay_us(50); // XXX Delay more?
	}
	if (current_station != prev_station)
		send_rfid(current_station);
	else 
		send_rfid(0);
	//line_init();
}


uint8_t compare_RFID_arrays(const uint8_t current_compare_RFID[12])
{
	uint8_t i;
	for (i = 2; i <= 10; ++i)
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
	/*
	else if (compare_RFID_arrays(RFID_B60))
	return 60;
	else if (compare_RFID_arrays(RFID_B61))
	return 61;
	else if (compare_RFID_arrays(RFID_B62))
	return 62;
	else if (compare_RFID_arrays(RFID_B63))
	return 63;
	else if (compare_RFID_arrays(RFID_B64))
	return 64;
	else if (compare_RFID_arrays(RFID_B65))
	return 65;
	else if (compare_RFID_arrays(RFID_B100))
	return 100;
	else if (compare_RFID_arrays(RFID_B101))
	return 101;
	else if (compare_RFID_arrays(RFID_B102))
	return 102;
	else if (compare_RFID_arrays(RFID_B103))
	return 103;
	else if (compare_RFID_arrays(RFID_B104))
	return 104;
	else if (compare_RFID_arrays(RFID_B105))
	return 105;
	*/
	else
	return 0; // no match
};
