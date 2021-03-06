/**
 *	@file RFID_scanner.c
 *	@author Erik Nybom
 *
 *	Functions for reading RFID scanner data
 */
#ifndef F_CPU
	#define F_CPU 20000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "RFID_scanner.h"
#include "../shared/usart.h"
#include "../shared/bus.h"
#include "../Sensor/linesensor.h"
#include "../shared/LCD_interface.h"
#include "rfid_tags.h"

uint8_t station_RFID[12];
uint8_t prev_station = 0;

/**
 *	Make RFID scanner ready for reading
 */
void RFID_scanner_init(void)
{
	DDRD = 0b00011100; // Set PD2 as Output XXX pd3 & pd4 for debugging
	PORTD |= (1 << PORTD2); // Disable reading
}

/**
 *	Read an RFID tag into the global variable station_RFID. This functions clears
 *	USART buffer before requesting data. Possible status codes are:
 *
 *	- 0 if successful
 *	- 1 if no tag was found
 *	- 2 if no start byte was found
 *	- 3 if start byte was invalid
 *	- 4 if all data bytes weren't read
 *	- 5 if no stop byte was found
 *	- 6 if stop byte was invalid
 *
 *	@return Status code
 */
uint8_t RFID_read_usart(void)
{
	usart_clear_buffer();
	PORTD &= ~(1 << PORTD2); // Enable reading
	uint8_t i;
	_delay_ms(150);
	for (i = 0; i < 25; i++) {
		_delay_ms(10);
		if (usart_has_bytes()) {
			break;
		}
	}
	// Check if answer was received
	if (i == 25) {
		PORTD |= (1 << PORTD2); // Disable reading
		display(1, "%u", UCSR0B); // XXX: Remove this?
		return 1;
	}

	// Wait for complete response and disable reading
	_delay_ms(50);
	PORTD |= (1 << PORTD2); // Disable reading

	// Read start byte
	if (usart_read_byte(&station_RFID[0])) {
		return 2;
	}

	if (station_RFID[0] != 0x0a) {
		return 3;
	}

	for (i = 1; i <= 10; ++i) // Read 12 bytes
	{
		if (usart_read_byte(&station_RFID[i]))
		{
			return 4;
		}
	}

	if (usart_read_byte(&station_RFID[11])) {
		return 5;
	}

	if (station_RFID[11] != 0x0d) {
		return 6;
	}

	return 0;
}

/**
 *	Reset station_RFID global
 */
void clear_station_RFID(void)
{
	uint8_t i = 0;
	for (i = 0; i <= 11; ++i)
	{
		station_RFID[i] = 0;
	}
}

/**
 *	Send given station tag to chassi
 *
 *	@param station_tag ID of RFID card
 */
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

/**
 *	Compare data in global station_RFID with given reference array of tag data
 *
 *	@param curren_compare_RFID RFID tag as defined in rfid_tags.h
 *
 *	@return 1 if tags match, else 0
 */
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

/**
 *	Identify the station currently in the station_RFID global
 *
 *	@return ID of station in station_RFID, or 0 if no match
 */
uint8_t identify_station_RFID(void)
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
	else
		return 0; // no match
};
