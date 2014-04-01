/*
 * sidescanner.c
 *
 * Created: 2014-04-01 09:30:00
 *  Author: Philip
 */ 

//constants
const uint8_t start_angle = 156;
const uint16_t end_angle = 780;
const double  step = (end_angle - start_angle) / 180;

void sidescanner_init()
{
	DDRB = 0b01100000;	//Set port direction

	// set top value
	ICR1H = 0x18; //Top value high (18 with prescaler 64)
	ICR1L = 0x69; // Top value low (6249 or 0x1869 with prescaler 64)

	// set WGM3:0 --> choose mode 14, fast pwm
	TCCR3A |= (1<< WGM11 | 0 << WGM10);
	TCCR3B |= (1 << WGM13 | 1 << WGM12);

	//set com1a com1b Clear OCnA/OCnB on Compare Match, set OCnA/OCnB at BOTTOM (non-inverting mode)
	TCCR3A |= (1 << COM1A1 | 0<<COM1A0 | 1<< COM1B1 | 0 << COM1B0);

	//set prescaler 64
	TCCR3B |= (0 << CS12 | 1 << CS11 | 1 << CS10);
}


void scanner_left_position(int angle)
{
	OCR3A = start_angle + step*angle;
}

void scanner_right_position(int angle)
{
	OCR3B = start_angle + step*angle;
}