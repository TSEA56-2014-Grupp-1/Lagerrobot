/*
 * automatic_steering.c
 *
 * Created: 2014-03-30 16:50:57
 *  Author: Lucas
 */ 

#include "automatic_steering.h"

void steering_algorithm ()
{

	if(prev_error < 0)
	{
		speed_right = 1300;
		speed_left = 1300 - (uint8_t)control;
		drive_left_wheels(speed_left);
		drive_right_wheels(speed_right);
	}
	
	else if (prev_error > 0)
	{
		speed_left = 1300;
		speed_right = 1300 - (uint8_t)control;
		drive_right_wheels(speed_right);
		drive_left_wheels(speed_left);
	}
	
	else if (prev_error == 0)
	{
		drive_right_wheels(1300);
		drive_left_wheels(1300);
	}
	
	else
	stop_wheels();
	
	
}


void pd_update(double curr_error, double dt)
{
	double diff;
	double p_term;
	double d_term;
	
	// differentiation
	diff = ((curr_error - prev_error) / dt);
	
	// scaling
	p_term = (proportional_gain * curr_error);
	d_term = (derivative_gain   * diff);
	
	// summation of terms
	control = abs(p_term) + d_term;
	
	// save current error as previous error for next iteration
	prev_error = curr_error;
}

void regulator_init()
{
	prev_error = 0;
	proportional_gain = 10;
	derivative_gain = 6;
}