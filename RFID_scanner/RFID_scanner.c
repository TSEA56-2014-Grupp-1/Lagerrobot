/*
 * GccApplication3.c
 *
 * Created: 2014-04-03 11:13:39
 *  Author: Erik
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../shared/usart.h"
#include "RFID_scanner.h"
uint8_t RFID_ID[11];


void init_RFID_scanner()
{
	sei(); // Enable interrupts
	DDRD = 0b0000100; // Set PD1 as Output
}

void read_RFID()
{
	PORTD = (1 << PORTD2);
	uint8_t i = 0;
	for (i = 0; i <= 11; ++i) // Read 12 bytes
	{
		usart_read_byte(&RFID_ID[i]);
	}
	PORTD = PORTD & (0 << PORTD2);
}

uint8_t compare_RFID()
{
	//uint8_t i;
	uint8_t j;
	uint8_t match;
	//for (i = 0; i <= 5; ++i)
//	{
		match = 1;
		j = 0;
		
		while (j <= 11 && match != 0)
		{
		if (RFID_B80[j] != RFID_ID[j])
			match = 0;
			++j;
		}
		if (match)
		return *RFID_B80;
	//}
	return 0;
}

int main(void)
{
	init_RFID_scanner();
	usart_init(520);

	read_RFID();
	

    while(1)
    {
        //TODO:: Please write your application code 
    }
}