/*
 * linesensor.h
 *
 * Created: 2014-03-27 09:15:08
 *  Author: Karl
 */ 


#ifndef LINESENSOR_H_
#define LINESENSOR_H_

void line_init();
void update_linesensor();
void update_linesensor_values();
void update_linesensor_surfaces();
void pickup_station_detection();
void line_break_detection();
void calculate_line_weight();
void calibrate_linesensor();
void calibrate_tape();
void calibrate_floor();

#endif /* LINESENSOR_H_ */