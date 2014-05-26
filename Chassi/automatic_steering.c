/**
 *	@file automatic_steering.c
 *	@author Lucas Nilsson
 *
 *	Calculates steering direction based on current robot position
 */

#include "automatic_steering.h"

/**
 *	Store previous error as set by pd_update()
 */
int8_t prev_error;

/**
 *	Bus callback for setting proportional gain in PD controller
 *
 *	@param id Unused
 *	@param kp_data Proportional gain to use
 */
void engine_set_kp(uint8_t id, uint16_t kp_data)
{
	proportional_gain = kp_data;
}

/**
 *	Bus callback for setting proportional gain in PD controller
 *
 *	@param id Unused
 *	@param kd_data Derivative gain to use
 */
void engine_set_kd(uint8_t id, uint16_t kd_data)
{
	derivative_gain = kd_data;
}

/**
 *	Calculate new steering based on passed current error and previous error
 *
 *	@param curr_error
 *
 *	@return Steering wheel position to use. Negative means turn left and positive
 *	        turn right.
 */
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

/**
 *	Set regulator constants to default values. To be used when initializing robot
 */
void regulator_init(void)
{
	prev_error = 0;
	proportional_gain = 180;
	derivative_gain = 5;
}
