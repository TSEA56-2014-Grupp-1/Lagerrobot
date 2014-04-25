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
void pickup_station_detection();
void calculate_line_weight();
void init_linesensor_calibration();

uint8_t get_sensor_surface(uint8_t sensor_id);

uint16_t return_line_weight(uint8_t id, uint16_t metadata);
uint16_t return_linesensor(uint8_t id, uint16_t sensor_pair);
uint16_t set_tape_reference(uint8_t id, uint16_t input_tape_reference);
void calibrate_linesensor(uint8_t id, uint16_t calibration_variable);

#endif /* LINESENSOR_H_ */