/*
 * automatic_steering.c
 *
 * Created: 2014-03-30 16:50:57
 *  Author: Lucas
 */

#include "automatic_steering.h"

/**
 *	Store previous error as set by pd_update()
 */
uint8_t prev_error;

void engine_set_kp(uint8_t id, uint16_t kp_data)
{
	proportional_gain = kp_data;
}

void engine_set_kd(uint8_t id, uint16_t kd_data)
{
	derivative_gain = kd_data;
}

int16_t pd_update(int8_t curr_error)
{
	double diff;
	int16_t p_term;
	double d_term;

	// differentiation
	diff = curr_error - prev_error;

	// scaling
	p_term = proportional_gain * curr_error;
	d_term = derivative_gain  * diff;

	// save current error as previous error for next iteration
	prev_error = curr_error;

	// Return regulated signal
	return p_term + floor(d_term);
}

void regulator_init(void)
{
	prev_error = 0;
	proportional_gain = 150;
	derivative_gain = 5;
}
