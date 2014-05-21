/*
 * automatic_steering.h
 *
 * Created: 2014-03-30 16:50:57
 *  Author: Lucas
 */
#ifndef AUTOMATIC_STEERING_H_
#define AUTOMATIC_STEERING_H_

#include "engine_control.h"

int16_t proportional_gain;
int16_t derivative_gain;

/**
 * @brief Sets the P-coefficient of the linefollower regulator.
 * @details Reacts to a bus transmit and sets the Proportional coefficient of the regulator.
 *
 * @param id Standard bus_receive parameter, id of the registered callback.
 * @param kp_data The value of the P-coefficient.
 */
void engine_set_kp(uint8_t id, uint16_t kp_data);

/**
 * @brief Sets the D-coefficient of the linefollower regulator.
 * @details Reacts to a bus transmit and sets the Derivative coefficient of the regulator.
 *
 * @param id Standard bus_receive parameter, id of the registered callback.
 * @param kd_data The value of the D-coefficient.
 */
void engine_set_kd(uint8_t id, uint16_t kd_data);

int16_t pd_update(int8_t curr_error);
void regulator_init(void);

#endif
