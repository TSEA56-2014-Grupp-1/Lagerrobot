/*
 * engine_control.h
 *
 * Created: 2014-03-30 16:50:57
 *  Author: Lucas
 */ 

#include "steering_commands.h"
#include "automatic_steering.h"

#define F_CPU 16000000UL
#include <avr/io.h>
#include <math.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

uint16_t line_weight;
double prev_error;
double proportional_gain;
double derivative_gain;
double control;
double curr_error;
uint16_t speed_left;
uint16_t speed_right;