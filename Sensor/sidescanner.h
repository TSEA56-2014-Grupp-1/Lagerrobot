/*
 * sidescanner.h
 *
 * Created: 4/3/2014 10:04:08 AM
 *  Author: phini326
 */ 


#ifndef SIDESCANNER_H_
#define SIDESCANNER_H_

#define SENSOR_SCANNER_ANGLE_START 156
#define SENSOR_SCANNER_ANGLE_END 780
#define SENSOR_SCANNER_ANGLE_STEP (SENSOR_SCANNER_ANGLE_END - SENSOR_SCANNER_ANGLE_START) / 180
#define ORIGO_TO_SCANNER_DISTANCE 160
#ifndef F_CPU
	#define F_CPU 20000000UL
#endif
#include <util/delay.h>

typedef uint8_t sensor;
enum sensor {sensor_arm, sensor_left, sensor_right};

void object_detection(uint8_t callback_id, uint16_t meta_data);
void sidescanner_init();


#endif /* SIDESCANNER_H_ */