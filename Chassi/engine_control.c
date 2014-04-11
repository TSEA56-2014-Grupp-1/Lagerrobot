/*
 * engine_control.c
 *
 * Created: 2014-03-27 10:59:57
 *  Author: Erik
 */ 

#include "engine_control.h"
#include "../shared/LCD_interface.h"


void engine_set_kp(uint8_t id, uint16_t kp_data)
{
	proportional_gain = kp_data;
	display(0, "kp %d", proportional_gain);
}

void engine_set_kd(uint8_t id, uint16_t kd_data)
{
	derivative_gain = kd_data;
	display(1, "kd %d", derivative_gain);
}


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

void engine_control_command(uint8_t checkout_id, uint16_t command_data)
{
	uint8_t command = (uint8_t)command_data;

	switch(command)
	{
		//STOP WHEELS
		case 0:
			stop_wheels();
			break;
		//Increase speed forwards
		case 1:
			if(driving_direction())
			{
				speed_left += STEERING_SPEED_INCREASE;
				speed_right += STEERING_SPEED_INCREASE;
			}
			else if ((speed_right >= STEERING_SPEED_INCREASE) | (speed_right >= STEERING_SPEED_INCREASE))
			{
				speed_left -= STEERING_SPEED_INCREASE;
				speed_right -= STEERING_SPEED_INCREASE;
			}
			else
			{
				drive_forward();
				speed_left = STEERING_SPEED_INCREASE;
				speed_right = STEERING_SPEED_INCREASE;
			}
			break;
		//Increase speed backwards, or break in forwarddirection
		case 2:
			if(!driving_direction())
			{
				speed_left += STEERING_SPEED_INCREASE;
				speed_right += STEERING_SPEED_INCREASE;
			}
			else if ((speed_right >= STEERING_SPEED_INCREASE) | (speed_right >= STEERING_SPEED_INCREASE))
			{
				speed_left -= STEERING_SPEED_INCREASE;
				speed_right -= STEERING_SPEED_INCREASE;
			}
			else
			{
				drive_backwards();
				speed_left = STEERING_SPEED_INCREASE;
				speed_right = STEERING_SPEED_INCREASE;
			}
			break;
		//Increase turningspeed to right, spin right if standing still
		case 3:
			if((speed_right < STEERING_SPEED_INCREASE) | (speed_right < STEERING_SPEED_INCREASE))
			{
				spin_right();
				speed_left = STEERING_SPIN_SPEED;
				speed_right = STEERING_SPIN_SPEED;
			}
			else if(driving_direction())
			{
				speed_right -= STEERING_SPEED_INCREASE;
			}
			else
			{
				speed_left -= STEERING_SPEED_INCREASE;
			}
			break;
		//Increase turningspeed to left, spin left if standing still
		case 4:
			if((speed_right < STEERING_SPEED_INCREASE) | (speed_right < STEERING_SPEED_INCREASE))
			{
				spin_left();
				speed_left = STEERING_SPIN_SPEED;
				speed_right = STEERING_SPIN_SPEED;
			}
			else if(driving_direction())
			{
				speed_left -= STEERING_SPEED_INCREASE;
			}
			else
			{
				speed_right -= STEERING_SPEED_INCREASE;
			}
			break;
		//TODO: Enable automatic steering
		//case 5:

	}

	drive_left_wheels(speed_left);
	drive_right_wheels(speed_right);

}