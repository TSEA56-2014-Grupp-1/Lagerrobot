/*
 * engine_control.c
 *
 * Created: 2014-03-27 10:59:57
 *  Author: Erik
 */ 

#include "engine_control.h"

void engine_init()
{
	//OCR1A = 0x0000; // = PD5 = Rightside wheels
	//OCR1B = 0x0000; // = PD4 = Leftside wheels
	//PD0 = DIR1, PD1 = DIR2
		
	DDRD = 0b00110011;	//Set port direction
	drive_forward(); // Initiate drive forward
		
	// set top value
	ICR1H = 0x07; //Top value high (00 with prescaler 64)  ( 0x07 with prescaler 8)
	ICR1L = 0xCF; // Top value low (249 or 0xF9 with prescaler 64) (1999 or 0x07CF with prescaler 8)
		
	// set WGM3:0 --> choose mode 14, fast pwm
	TCCR1A |= (1<< WGM11 | 0 << WGM10);
	TCCR1B |= (1 << WGM13 | 1 << WGM12);
		
	//set com1a com1b Clear OCnA/OCnB on Compare Match, set OCnA/OCnB at BOTTOM (non-inverting mode)
	TCCR1A |= (1 << COM1A1 | 0<<COM1A0 | 1<< COM1B1 | 0 << COM1B0);
		
	//set prescaler 8
	TCCR1B |= (0 << CS12 | 1 << CS11 | 0 << CS10);
}
