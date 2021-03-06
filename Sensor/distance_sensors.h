#ifndef DISTANCE_SENSORS_H_
#define DISTANCE_SENSORS_H_
/**
 *	@file distance_sensors.h
 *	@author Philip Nilsson
 *
 *	Utility functions for interpreting distance sensor data
 */

#include <avr/io.h>
#include <avr/interrupt.h>

typedef uint8_t sensor;
enum sensor {sensor_arm = 1, sensor_left, sensor_right};

uint16_t get_distance_sensor(int8_t, sensor);

uint16_t ad_interpolate(uint16_t ad_in, sensor sensor_id);

#endif /* DISTANCE_SENSORS_H_ */
