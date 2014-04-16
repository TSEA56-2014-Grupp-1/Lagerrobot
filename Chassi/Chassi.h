/*
 * Chassi.h
 *
 * Created: 3/31/2014 1:38:34 PM
 *  Author: johli887
 */ 


#ifndef CHASSI_H_
#define CHASSI_H_

#define F_CPU 16000000UL
#include <math.h>
#include <util/delay.h>
#include <stdlib.h>

// Decisions
const uint8_t AT_STATION = 1;
const uint8_t WRONG_STATION_MEM = 2;
const uint8_t WRONG_STATION_RFID = 3;
const uint8_t OBJECT_IS_PUT_DOWN = 4;
const uint8_t LOOK_FOR_OBJECT = 5;

// Global variables
uint8_t carrying_rfid = 0;
uint8_t first_lap_done = 0;
uint8_t station_list[100];
uint8_t station_count = 0;
// Functions
uint16_t request_line_data(); // send command to Sensor
uint8_t is_station(uint8_t);
uint16_t request_rfid_tag();
void arm_is_done(uint8_t id, uint16_t pickup_data);
void send_to_arm(uint16_t arm_action);
/*
void disable_rfid_reader();
void enable_rfid_reader();

void follow_line(uint8_t line_data)
uint8_t is_right_station_mem()
uint8_t save_time_and_rfid(uint8_t carrying_rfid) 
*/

#endif /* CHASSI_H_ */