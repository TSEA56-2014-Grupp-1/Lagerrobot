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
uint64_t Time_since_start = 0;
// Functions
uint16_t request_line_data(); // send command to Sensor
uint8_t is_station(uint8_t);
/*
void disable_rfid_reader();
void enable_rfid_reader();

void follow_line(uint8_t line_data)
void stop_wheels()
void send_decision_to_pc(uint8_t decision)// send data to pc
void send_rfid_to_pc(uint8_t rfid, uint8_t time_for_station)// send data to pc
uint8_t is_right_station_mem()
uint8_t save_time_and_rfid(uint8_t carrying_rfid) 

uint32_t request_rfid_data() // send command to Sensor
void put_down_object() // send command to Arm
void pick_up_object() // send command to Arm
uint8_t benefit_turn_around()
void turn_around() 
*/

#endif /* CHASSI_H_ */