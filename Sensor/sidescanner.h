/*
 * sidescanner.h
 *
 * Created: 4/3/2014 10:04:08 AM
 *  Author: phini326
 */ 


#ifndef SIDESCANNER_H_
#define SIDESCANNER_H_

void sidescanner_init();
uint8_t scanner_left_position(uint8_t);
uint8_t scanner_right_position(uint8_t);
uint8_t sweep_left();
uint8_t sweep_right();

void update_distance();


#endif /* SIDESCANNER_H_ */