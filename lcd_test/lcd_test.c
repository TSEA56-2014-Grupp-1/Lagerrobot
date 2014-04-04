/*
 * lcd_test.c
 *
 * Created: 3/28/2014 4:34:09 PM
 *  Author: karli315
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "../shared/bus.h"
#include "../shared/LCD_interface.h"

#include <avr/interrupt.h>

int main(void)
{
	bus_init(BUS_ADDRESS_ARM);
	
	lcd_interface_init();
	
	
	sei();
	_delay_ms(200);
	display_text("Jag ar en Arm", "Hihihihi");
	//display_numbers(100, 1337);
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}