/*
 * motorkontroll.c
 *
 * Created: 2014-03-27 10:59:57
 *  Author: Erik
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <math.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void drive_left_wheels(int speed)
{
	OCR1B = speed;
}

void drive_right_wheels(int speed)
{
	OCR1A = speed;
}

void drive_forward()
{
	PORTD = (1 << PORTD0);
	PORTD = (0 << PORTD1);
}

void drive_backwards()
{
	PORTD = (0 << PORTD0);
	PORTD = (1 << PORTD1);
	
}
void spinn_right()
{
	PORTD = (1 << PORTD0);
	PORTD = (1 << PORTD1);
}

void spinn_left()
{
	PORTD = (0 << PORTD0);
	PORTD = (0 << PORTD1);
}
void stop_wheels()
{
	drive_left_wheels(0);
	drive_right_wheels(0);
}
void wait_wheels(int milisec)
{
	for (int i = 1; i < milisec; i++)
	{
		_delay_ms(100);
	}
}
int main(void)
{
	//OCR1A = 0x0000; // = PD5 = Rightside wheels
	//OCR1B = 0x0000; // = PD4 = Leftside wheels
	//PD0 = DIR1
	//PD1 = DIR2
	
	
	DDRD = 0b00110011;
	PORTD = 0b00000001; // Initiate drive forward
	// set top value
	ICR1H = 0x00; //Topvalue high
	ICR1L = 0xF9; // Topvalue low (249 with prescaler 64)
	
	// set WGM3:0 --> choose mode 14, fast pwm
	TCCR1A |= (1<< WGM11 | 0 << WGM10);
	TCCR1B |= (1 << WGM13 | 1 << WGM12);
	
	//set com1a com1b Clear OCnA/OCnB on Compare Match, set OCnA/OCnB at BOTTOM (non-inverting mode)
	TCCR1A |= (1 << COM1A1 | 0<<COM1A0 | 1<< COM1B1 | 0 << COM1B0);
	
	//set prescaler 64
	TCCR1B |= (0 << CS12 | 1 << CS11 | 1 << CS10);

		drive_left_wheels(50);
		drive_right_wheels(50);
		wait_wheels(10);
		stop_wheels();
		wait_wheels(10);

		drive_backwards();
		drive_left_wheels(249);
		drive_right_wheels(249);
		wait_wheels(10);
		stop_wheels();
		wait_wheels(10);
		spinn_left();
		drive_left_wheels(249);
		drive_right_wheels(249);
		wait_wheels(10);
		stop_wheels();
		
	while(1)
    {
        //TODO:: Please write your application code 
    }
}