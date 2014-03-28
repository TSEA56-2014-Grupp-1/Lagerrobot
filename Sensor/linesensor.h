/*
 * linesensor.h
 *
 * Created: 2014-03-27 09:15:08
 *  Author: Karl
 */ 


#ifndef LINESENSOR_H_
#define LINESENSOR_H_

void line_init();
void update_linsensor_values();
void pickup_station_check();

uint8_t calculate_line_weight();

#endif /* LINESENSOR_H_ */