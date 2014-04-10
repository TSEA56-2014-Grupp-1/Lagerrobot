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
#include "RFID_scanner.h"
#include "../shared/usart.h"
#include "../shared/bus.h"
#include "../shared/LCD_interface.h"

uint8_t check_error = 0;
uint8_t station_RFID[12];
uint8_t carrying_RFID[12];
uint8_t tag = 15;

void RFID_scanner_init()
{
	DDRD = 0b11100100; // Set PD2 as Output and pd6 as output for testing XXX
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
	//PORTD = (0 << PORTD2); // Enable reading
	//_delay_ms(200);
	//_delay_ms(100);
	uint8_t i = 0;
	
	for (i = 0; i <= 11; ++i) // Read 12 bytes
	{
		check_error = usart_read_byte(&station_RFID[i]);
		if (check_error == 1)
		break;
	}
	_delay_ms(100);
	//PORTD |= (1 << PORTD2); // Disable reading
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

int main(void)
{
	RFID_scanner_init();
	//PORTD = 0;
	DDRA = 0b11111111; //For testing
	PORTA = 0; // For testing
	usart_init(520);
	sei();

	PORTD = (0 << PORTD2); // Enable reading
	_delay_ms(200);
	_delay_ms(100);	
	read_RFID();
	_delay_ms(100);
	read_RFID();
	_delay_ms(100);
	PORTD |= (1 << PORTD2); // Disable reading
	tag = identify_station_RFID();
	PORTA = tag;
	
	/* //TESTLOOP
	for (int i = 0; i < 9; ++i) // For testing
	{
	read_RFID(); // At least 2 readings are needed!
	tag = 15;
	tag = identify_station_RFID();
	station_RFID[1] = 0x0B;
	PORTA = tag; // For testing
	PORTA |= PORTA & (check_error << PORTA7);
	_delay_ms(200);
	_delay_ms(200);
	_delay_ms(200);
	_delay_ms(200);
	_delay_ms(200);
	_delay_ms(200);
	_delay_ms(200);
	PORTA = 0;
	}
	*/
	
	
//	station_to_LCD(tag);
    while(1)
    {
        //TODO:: Please write your application code 
    }
}
