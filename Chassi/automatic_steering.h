/*
 * automatic_steering.h
 *
 * Created: 2014-03-30 16:50:57
 *  Author: Lucas
 */

#include "engine_control.h"

void steering_algorithm();
void pd_update(double curr_error, double dt);
void regulator_init();