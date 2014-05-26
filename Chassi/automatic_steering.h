#ifndef AUTOMATIC_STEERING_H_
#define AUTOMATIC_STEERING_H_
/**
 *	@file automatic_steering.h
 *	@author Lucas Nilsson
 *
 *	Calculates steering direction based on current robot position
 */

#include "engine_control.h"

int16_t proportional_gain;
int16_t derivative_gain;

void engine_set_kp(uint8_t id, uint16_t kp_data);
void engine_set_kd(uint8_t id, uint16_t kd_data);
int16_t pd_update(int8_t curr_error);
void regulator_init(void);

#endif
