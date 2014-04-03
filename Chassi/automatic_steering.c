/*
 * automatic_steering.c
 *
 * Created: 2014-03-30 16:50:57
 *  Author: Lucas
 */ 

#include "automatic_steering.h"

void steering_algorithm ()
{


	if(STEERING_MAX_SPEED > control && control > 0)
	{
		speed_left = STEERING_MAX_SPEED - abs(control);
		speed_right = STEERING_MAX_SPEED;
	}
	
	else if (-STEERING_MAX_SPEED < control && control < 0) {
		
		speed_right = STEERING_MAX_SPEED - abs(control);
		speed_left = STEERING_MAX_SPEED;
	}
	
	else if (control < 0)
	{
		speed_right = 0;
		speed_left = STEERING_MAX_SPEED;
	}
	
	else if (control > 0) {
		speed_right = STEERING_MAX_SPEED;
		speed_left = 0;
	}
	
	else if (control == 0)
	{
		speed_left = STEERING_MAX_SPEED;
		speed_right = STEERING_MAX_SPEED;
	}
	
	else {
		stop_wheels();
	}
	
	
	drive_left_wheels(speed_left);
	drive_right_wheels(speed_right);
	
}


void pd_update(int8_t curr_error, double dt)
{
	double diff;
	int16_t p_term;
	double d_term;
	
	// differentiation
	diff = ((curr_error - prev_error) / dt);
	
	// scaling
	p_term = (proportional_gain * curr_error);
	d_term = (derivative_gain  * diff);
	
	// summation of terms
	control = p_term + floor(d_term);
	
	// save current error as previous error for next iteration
	prev_error = curr_error;
}

void regulator_init()
{
	prev_error = 0;
	proportional_gain = 0;
	derivative_gain = 0;
}