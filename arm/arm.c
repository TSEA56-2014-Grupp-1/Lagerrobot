/*
 * Armenhet.c
 *
 * Created: 3/25/2014 4:23:12 PM
 *  Author: eriny778
 */

#if !defined(F_CPU)
	#define F_CPU 16000000UL
 #endif

#include <avr/io.h>
#include "arm.h"
#include "servo.h"

#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//------Functions-------

void arm_init()
{
	servo_transfer_command(
		SERVO_INST_WRITE,
		servo_make_command(
			SERVO_BROADCASTING_ID,
			SERVO_CW_COMPLIANCE_MARGIN,
			4,
			(uint8_t[]){0x03, 0x03, 0x40, 0x40}));

	servo_transfer_command(
		SERVO_INST_WRITE,
		servo_make_command(
			SERVO_BROADCASTING_ID,
			SERVO_GOAL_SPEED_L,
			2,
			(uint8_t[]){0x80, 0x00}));
}


void arm_move_joint_add(uint8_t joint, uint8_t goal_angle)
{
	switch (joint)
	{
		case 1:
			servo_buffer_move (0x01, goal_angle);
			break;
		case 2:
			servo_buffer_move (0x02, goal_angle);
			servo_buffer_move (0x03, 300-goal_angle);
			break;
		case 3:
			servo_buffer_move (0x04, goal_angle);
			servo_buffer_move (0x05, 180);
			break;
		case 4:
			servo_buffer_move (0x06, 180);
			break;
		case 5:
			servo_buffer_move (0x07, 180);
			break;
		case 6:
			servo_buffer_move (0x08, 180);
			break;
	}
}

int main(void)
{
	sei();
	DDRB = 0x00;
	PORTB = 0x00;
	PORTB = 0x01;
	while(1) {PORTB ^= 1;}
	servo_init();
	arm_init();

/* 	move_servo (0x07, 180);
	move_servo(0x06, 275);
// 	_delay_ms(240);
 	move_servo(0x01, 180);

	_delay_ms(700);
	_delay_ms(700);
	_delay_ms(700);
	_delay_ms(700);

	move_servo(0x02, 180);

	_delay_ms(700);
	_delay_ms(700);
	_delay_ms(700);
	_delay_ms(700);
*/
	while(1)
    {
/*		//dance routine
		move_servo(0x01, 270);
		move_servo(0x07, 90);
		_delay_ms(700);
		_delay_ms(700);
		_delay_ms(700);
		_delay_ms(700);
		move_servo(0x01, 90);
		move_servo(0x07, 270);
		_delay_ms(700);
		_delay_ms(700);
		_delay_ms(700);
		_delay_ms(700);*/
    }

}
