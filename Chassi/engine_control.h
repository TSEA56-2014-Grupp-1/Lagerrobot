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

uint16_t line_weight;
int prev_error;
int16_t proportional_gain;
int16_t derivative_gain;
int16_t control;
//double curr_error;
uint16_t speed_left;
uint16_t speed_right;

void engine_init();

#endif