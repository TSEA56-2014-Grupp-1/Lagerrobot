/*
 * steering_commands.h
 *
 * Created: 2014-03-30 16:50:57
 *  Author: Lucas
 */ 

#ifndef STEERING_COMMANDS_H_
#define STEERING_COMMANDS_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

void drive_left_wheels(uint16_t speed);
void drive_right_wheels(uint16_t speed);
void drive_forward();
void drive_backwards();
void spin_right();
void spin_left();
void stop_wheels();
void wait_wheels(int tenth_secs);
void turn_right(int speed);
void turn_left(int speed);
uint8_t is_driving();
uint8_t driving_direction();
uint8_t is_spinning();
uint8_t spinning_direction();
uint8_t is_moving();

#endif