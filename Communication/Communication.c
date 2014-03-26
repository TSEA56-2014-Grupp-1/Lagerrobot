/*
 * Communication.c
 *
 * Created: 2014-03-26 09:49:31
 *  Author: Karl
 */ 


#include <avr/io.h>
#include "LCD.h"

int main(void)
{
	lcd_init();
	
	lcd_print("Ciao bella!", "lololol");
    while(1)
    {
        //TODO:: Please write your application code 
    }
}