/*
 * sidescanner.h
 *
<<<<<<< HEAD
 * Created: 4/3/2014 10:04:08 AM
 *  Author: phini326
=======
 * Created: 4/2/2014 2:14:26 PM
 *  Author: johli887
>>>>>>> origin/sidescanner
 */ 


#ifndef SIDESCANNER_H_
#define SIDESCANNER_H_

#define SENSOR_SCANNER_ANGLE_START 156
#define SENSOR_SCANNER_ANGLE_END 780
#define SENSOR_SCANNER_ANGLE_STEP (SENSOR_SCANNER_ANGLE_END - SENSOR_SCANNER_ANGLE_START) / 180
#define ORIGO_TO_SCANNER_DISTANCE 30
#define F_CPU 20000000UL
#include <util/delay.h>

void sidescanner_init();
uint8_t scanner_left_position(uint8_t);
uint8_t scanner_right_position(uint8_t);
uint8_t sweep_left();
uint8_t sweep_right();
void wait_scanner_servo();
void update_distance();
void calculate_coordinates();



#endif /* SIDESCANNER_H_ */