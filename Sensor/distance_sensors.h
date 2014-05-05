/*
 * distance_sensors.h
 *
 * Created: 4/5/2014 3:50:01 PM
 *  Author: phini326
 */ 


#ifndef DISTANCE_SENSORS_H_
#define DISTANCE_SENSORS_H_

#include <avr/io.h>	

uint16_t get_distance_sensor_1(int8_t);
uint16_t get_distance_sensor_2(int8_t);
uint16_t get_distance_sensor_3(int8_t);

uint16_t ad_interpolate(uint16_t ad_in, uint8_t sensor);
	
#endif /* DISTANCE_SENSORS_H_ */