/*
 * automatic_steering.c
 *
 * Created: 2014-03-30 16:50:57
 *  Author: Lucas
 */ 

#include "automatic_steering.h"

void steering_algorithm ()
{

	if(prev_error > 0)
	{
		
		if (control > 1000)
		{
			speed_left = 0;
		}
		else {
			speed_left = 1000 - control;
		}
		speed_right = 1000;
		
		drive_left_wheels(speed_left);
		drive_right_wheels(speed_right);
	}
	
	else if (prev_error < 0)
	{
		if (control > 1000)
		{
			speed_right = 0;
		}
		else {
			speed_right = 1000 - control;
		}
		speed_left = 1000;
		
		drive_right_wheels(speed_right);
		drive_left_wheels(speed_left);
	}
	
	else if (prev_error == 0)
	{
		drive_right_wheels(200);
		drive_left_wheels(200);
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
	d_term = (derivative_gain  * diff);
	
	// summation of terms
	control = abs(p_term) + d_term;
	
	// save current error as previous error for next iteration
	prev_error = curr_error;
}

void regulator_init()
{
	prev_error = 0;
	proportional_gain = 50;
	derivative_gain = 0;
}