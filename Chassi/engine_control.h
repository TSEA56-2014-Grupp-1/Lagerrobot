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

// Constants controlling how much the accelerator and steering
// increases or decreases with each control input.
#define STEERING_THRUST_INCREASE 300
#define STEERING_TURN_INCREASE 200

#define STEERING_MAX_SPEED 1999L

#define CMD_STOP 0
#define CMD_SPEED_UP 1
#define CMD_SPEED_DOWN 2
#define CMD_RIGHT 3
#define CMD_LEFT 4

/**
 * @brief Initializes the motor control.
 * @details Sets up the timers for PWM usage and sets the wheel direction to forward.
 */
void engine_init(void);

/**
 * @brief Executes an engine control order.
 * @details Reacts to a bus transmit and updates the steering and speed parameters
 * according to the supplied order
 *
 * @param id Standard bus_receive parameter, id of the registered callback.
 * @param command_data Contains an identifier for a control command.
 */
void engine_control_command(uint8_t id, uint16_t command_data);

/**
 * @brief Updates the left wheels with a direction and a speed.
 *
 * @param direction The direction the wheels should turn. 1 is forward, 0 is backward.
 * @param speed The speed at which the wheels should turn.
 */
void drive_left_wheels(uint8_t direction, uint16_t speed);

/**
 * @brief Updates the right wheels with a direction and a speed.
 *
 * @param direction The direction the wheels should turn. 1 is forward, 0 is backward.
 * @param speed The speed at which the wheels should turn.
 */
void drive_right_wheels(uint8_t direction, uint16_t speed);

/**
 * @brief Stops the wheels.
 */
void stop_wheels(void);

/**
 * @brief Updates the wheel speeds and directions.
 * @details Sets the speed and direction of each wheelpair according to the current steering and accelerometer settings.
 */
void update_steering(int16_t steering_wheel, uint16_t speed);

int16_t get_steering_wheel(void);

int16_t get_speed(void);

#endif
