/*
 * motorkontroll.c
 *
 * Created: 2014-03-27 10:59:57
 *  Author: Erik
 */ 


#include <avr/io.h>
#include <math.h>
#include <avr/interrupt.h>

void drive_right_wheels(int speed)
{
	TCCR1B = 0;
	uint16_t round_speed = (uint16_t)(round(speed*159.99));
	OCR1B = round_speed;
	TCCR1B = 0xA2;
}
void drive_left_wheels(int speed)
{
	//TCCR1A = 0;
	uint16_t round_speed = (uint16_t)(round(speed*159.99));
	OCR1A = round_speed;
	//TCCR1A = 0xA2;
	
}

int main(void)
{
	//TIMSK1 = (1 << OCIE1B | 1 << OCIE1A); // Enable timer interrupts 
	DDRD |= 0b00011000;
	//PORTD |= 0b00011000;
	OCR1A = 0x1000;
	ICR1H = 0x3E;
	ICR1L = 0x7F;
	TCCR1B = 0b00011001;// 0x19; //set to fast pwm
	TCCR1A = 0b10100010; // xA2; //COMA = 10, COMB = 10 -> PD4 & PD5 outports
	//ICR = 3E7F (=15999)
	//sei();
	//drive_left_wheels(60);

	while(1)
    {
		OCR1A = 0x1000;
        //TODO:: Please write your application code 
    }
}