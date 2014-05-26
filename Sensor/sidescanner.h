#ifndef SIDESCANNER_H_
#define SIDESCANNER_H_
/**
 *	@file sidescanner.h
 *	@author Johan Lind and Philip Nilsson
 *
 *	Functions for handling robot side scanners
 */

#ifndef F_CPU
	#define F_CPU 20000000UL
#endif

#include "distance_sensors.h"
#include <util/delay.h>

/**
 *	PWM value for left scanner which is considered 0 degrees
 */
#define SENSOR_SCANNER_LEFT_ANGLE_START 250

/**
 *	PWM value for left scanner which is considered 180 degrees
 */
#define SENSOR_SCANNER_LEFT_ANGLE_END 750

/**
 *	How much to increase left scanner PWM value for each step (1 degree)
 */
#define SENSOR_SCANNER_LEFT_ANGLE_STEP (SENSOR_SCANNER_LEFT_ANGLE_END - SENSOR_SCANNER_LEFT_ANGLE_START) / 1.8

/**
 *	PWM value for right scanner which is considered 0 degrees
 */
#define SENSOR_SCANNER_RIGHT_ANGLE_START 270

/**
 *	PWM value for right scanner which is considered 180 degrees
 */
#define SENSOR_SCANNER_RIGHT_ANGLE_END 756

/**
 *	How much to increase right scanner PWM value for each step (1 degree)
 */
#define SENSOR_SCANNER_RIGHT_ANGLE_STEP (SENSOR_SCANNER_RIGHT_ANGLE_END - SENSOR_SCANNER_RIGHT_ANGLE_START) / 1.8

/**
 *	First angle in degrees to scan. This prevents detecting wheels
 */
#define SENSOR_SCANNER_ANGLE_FIRST 25

/**
 *	Last angle in degrees to scan before giving up. This prevents detecting wheels
 */
#define SENSOR_SCANNER_ANGLE_LAST 180 - SENSOR_SCANNER_ANGLE_FIRST

/**
 *	Distance in mm from scaner axis to arm rotation axis
 */
#define ORIGO_TO_SCANNER_DISTANCE_LEFT  115

/**
 *	Distance in mm from scaner axis to arm rotation axis
 */
#define ORIGO_TO_SCANNER_DISTANCE_RIGHT 115

/**
 *	Distance from sensor edge to servo rotation axis
 */
#define SCANNER_AXIS_TO_FRONT           35

void object_detection(sensor sensor_id);
void sidescanner_init(sensor sensor_id);
uint8_t scanner_set_position(uint8_t angle, sensor sensor_id);

#endif /* SIDESCANNER_H_ */
