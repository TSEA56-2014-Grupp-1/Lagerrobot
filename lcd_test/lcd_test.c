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
	bus_init(BUS_ADDRESS_CHASSIS);
	
	lcd_interface_init();
	
	
	sei();
	_delay_ms(200);
	display(0, "ABCD");
	display(1, "123A%.2f", 2.34);
    while(1)
    {
        //TODO:: Please write your application code 
    }
}