#ifndef LINESENSOR_H_
#define LINESENSOR_H_
/**
 *	@file linesensor.h
 *	@author Karl Linderhed and Philip Nilsson
 *
 *	Functions for reading and interpreting line sensor data
 */

void line_init();
uint16_t send_line_data(uint8_t id, uint16_t metadata);
void update_linesensor();
void update_linesensor_values();
void pickup_station_detection();
void calculate_line_weight();
void init_linesensor_calibration();
uint8_t get_sensor_surface(uint8_t sensor_id);
uint16_t return_line_weight(uint8_t id, uint16_t metadata);
uint16_t return_linesensor(uint8_t id, uint16_t sensor_pair);
uint16_t set_tape_reference(uint8_t id, uint16_t input_tape_reference);
uint8_t get_tape_width();
uint8_t is_tape_right();
uint8_t is_tape_left();
uint8_t line_read_sensor(uint8_t sensor_id);
void calibrate_linesensor(uint8_t id, uint16_t metadata);
void clear_pickupstation();
uint8_t get_station_data();

#endif /* LINESENSOR_H_ */
