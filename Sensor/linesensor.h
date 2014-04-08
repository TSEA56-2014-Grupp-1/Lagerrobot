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
void calibrate_tape();
void calibrate_floor();
void init_linesensor_calibration();

uint16_t return_line_weight(uint8_t id, uint16_t metadata);
uint16_t return_linesensor(uint8_t id, uint16_t sensor);
uint16_t set_tape_reference(uint8_t id, uint16_t input_tape_reference);
uint16_t calibrate_linesensor(uint8_t id, uint16_t calibration_variable);

#endif /* LINESENSOR_H_ */