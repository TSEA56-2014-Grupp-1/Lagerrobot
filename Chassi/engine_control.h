/*
 * engine_control.h
 *
 * Created: 2014-03-30 16:50:57
 *  Author: Lucas
 */ 

#ifndef ENGINE_CONTROL_H_
#define ENGINE_CONTROL_H_


#include "steering_commands.h"
#include "automatic_steering.h"

#define F_CPU 16000000UL
#include <avr/io.h>
#include <math.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#define STEERING_SPEED_INCREASE 150
#define STEERING_SPIN_SPEED 500

uint16_t line_weight;
uint8_t prev_error;
int16_t proportional_gain;
int16_t derivative_gain;
int16_t control;
uint16_t speed_left;
uint16_t speed_right;

void engine_set_kp(uint8_t id, uint16_t kp_data);
void engine_set_kd(uint8_t id, uint16_t kd_data);
void engine_init();
void engine_control_command(uint8_t checkout_id, uint16_t command_data);

#endif