/*
 * automatic_steering.c
 *
 * Created: 2014-03-30 16:50:57
 *  Author: Lucas
 */ 

#include "automatic_steering.h"	
	
void steering_algorithm ()
{
	steering_wheel = control;
	update_steering();
	
}


void pd_update(int8_t curr_error)
{	
	double diff;
	int16_t p_term;
	double d_term;
	//double dt = 0.0167;
	
	// differentiation
	diff = (curr_error - prev_error);
	
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
	proportional_gain = 150;
	derivative_gain = 5;
}
