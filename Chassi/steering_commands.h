/*
 * steering_commands.h
 *
 * Created: 2014-03-30 16:50:57
 *  Author: Lucas
 */ 

#include <avr/io.h>
#include <util/delay.h>

void drive_left_wheels(int speed);
void drive_right_wheels(int speed);
void drive_forward();
void drive_backwards();
void spin_right();
void spin_left();
void stop_wheels();
void wait_wheels(int tenth_secs);
void turn_right(int speed);
void turn_left(int speed);
