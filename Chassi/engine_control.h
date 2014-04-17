/*
 * engine_control.h
 *
 * Created: 2014-03-30 16:50:57
 *  Author: Lucas
 */ 

#ifndef ENGINE_CONTROL_H_
#define ENGINE_CONTROL_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <math.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "automatic_steering.h"

#define STEERING_THRUST_INCREASE 300
#define STEERING_TURN_INCREASE 200
#define STEERING_SPIN_SPEED 500

#define STEERING_MAX_SPEED 1999L

uint16_t line_weight;
uint8_t prev_error;
int16_t proportional_gain;
int16_t derivative_gain;
int16_t control;
int16_t steering_wheel;
int16_t accelerator;

void engine_set_kp(uint8_t id, uint16_t kp_data);
void engine_set_kd(uint8_t id, uint16_t kd_data);
void engine_init();
void engine_control_command(uint8_t checkout_id, uint16_t command_data);

void drive_left_wheels(uint8_t direction, uint16_t speed);
void drive_right_wheels(uint8_t direction, uint16_t speed);
void stop_wheels();
void wait_wheels(int tenth_secs);

void update_steering();



#endif