/*
 * distance_sensors.h
 *
 * Created: 4/5/2014 3:50:01 PM
 *  Author: phini326
 */ 


#ifndef DISTANCE_SENSORS_H_
#define DISTANCE_SENSORS_H_

#include <avr/io.h>



const uint16_t DISTANCE_SENSOR_1[31] = {610, 610, 598, 547, 495, 456, 416, 388, 359, 338,
											319, 301, 286, 271, 260, 249, 240, 229, 221, 212, 204,
											195, 187, 183, 175, 171, 167, 160, 156, 153, 149};
const uint16_t DISTANCE_SENSOR_2[26] = {457, 399, 349, 313, 283, 264, 238, 219, 205, 194,
											185, 170, 165, 152, 144, 140, 132, 128, 120, 116,
											115, 107, 103, 100, 96, 92};
const uint16_t DISTANCE_SENSOR_3[26] = {437, 390, 334, 297, 264, 236, 217, 204, 189, 173,
											161, 154, 142, 134, 126, 122, 115, 111, 103, 99,
											95, 92, 87, 84, 83, 79};


	
#endif /* DISTANCE_SENSORS_H_ */