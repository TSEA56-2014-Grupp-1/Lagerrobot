/*
 * lcd_test.c
 *
 * Created: 2014-05-02 14:36:56
 *  Author: Karl
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "../shared/bus.h"
#include "../shared/LCD_interface.h"

int main(void)
{
	DDRB = 0xff;
	bus_init(BUS_ADDRESS_CHASSIS);
	_delay_ms(500);
	
	display(0, "");
	display(1, "3");
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}