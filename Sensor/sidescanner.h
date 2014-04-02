/*
 * sidescanner.h
 *
 * Created: 4/2/2014 2:14:26 PM
 *  Author: johli887
 */ 


#ifndef SIDESCANNER_H_
#define SIDESCANNER_H_

#define F_CPU 20000000UL
#include <util/delay.h>

//constants
#define SENSOR_SCANNER_ANGLE_START 156
#define SENSOR_SCANNER_ANGLE_END 780
#define SENSOR_SCANNER_ANGLE_STEP (SENSOR_SCANNER_ANGLE_END - SENSOR_SCANNER_ANGLE_START) / 180

void sidescanner_init();
void scanner_left_position(int angle);
void scanner_right_position(int angle);
void wait_scanner_servo();
void scan_left_side();
void scan_right_side();

#endif /* SIDESCANNER_H_ */