/*
 * sidescanner.h
 *
 * Created: 4/3/2014 10:04:08 AM
 *  Author: phini326
 */


#ifndef SIDESCANNER_H_
#define SIDESCANNER_H_

#include "distance_sensors.h"

#define SENSOR_SCANNER_LEFT_ANGLE_START 250
#define SENSOR_SCANNER_LEFT_ANGLE_END 750
#define SENSOR_SCANNER_LEFT_ANGLE_STEP (SENSOR_SCANNER_LEFT_ANGLE_END - SENSOR_SCANNER_LEFT_ANGLE_START) / 1.8
#define SENSOR_SCANNER_ANGLE_FIRST 25
#define SENSOR_SCANNER_ANGLE_LAST 180 - SENSOR_SCANNER_ANGLE_FIRST
#define SENSOR_SCANNER_RIGHT_ANGLE_START 270
#define SENSOR_SCANNER_RIGHT_ANGLE_END 756
#define SENSOR_SCANNER_RIGHT_ANGLE_STEP (SENSOR_SCANNER_RIGHT_ANGLE_END - SENSOR_SCANNER_RIGHT_ANGLE_START) / 1.8

#define ORIGO_TO_SCANNER_DISTANCE_LEFT  115
#define ORIGO_TO_SCANNER_DISTANCE_RIGHT 115
#define SCANNER_AXIS_TO_FRONT           35
#ifndef F_CPU
	#define F_CPU 20000000UL
#endif
#include <util/delay.h>

void object_detection(sensor sensor_id);
void sidescanner_init(sensor sensor_id);
uint8_t scanner_set_position(uint8_t angle, sensor sensor_id);

#endif /* SIDESCANNER_H_ */
