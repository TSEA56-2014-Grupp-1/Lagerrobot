﻿/*
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
#define AT_STATION 1;
#define WRONG_STATION_MEM 2;
#define WRONG_STATION_RFID 3;
#define OBJECT_IS_PUT_DOWN 4;
#define LOOK_FOR_OBJECT 5;

// Global variables
uint8_t carrying_rfid;
uint8_t first_lap_done;
uint8_t station_list[100];
uint8_t station_count;
uint8_t follow_line;
uint8_t	manual_control;

/**
 * @brief Requests line data from the sensor unit.
 * @details Performs a bus request to the sensor unit for data on the center
 * of mass and station information.
 * @return The data returned by the Sensor unit. High byte contains station information, 
 * low byte contains the value of the center of mass for the line sensor.
 */
uint16_t request_line_data();

/**
 * @brief Checks if provided station information indicates a station.
 * 
 * @param station_data The station information to be checked. 0 or 2 indicates a station.
 * @return 1 if the information indicates a station, 0 otherwise.
 */
uint8_t is_station(uint8_t station_data);

uint16_t request_rfid_tag();
void arm_is_done(uint8_t id, uint16_t pickup_data);
void send_to_arm(uint16_t arm_action);

uint16_t got_steering_request(uint8_t id, uint16_t metadata);
/*
void disable_rfid_reader();
void enable_rfid_reader();

void follow_line(uint8_t line_data)
uint8_t is_right_station_mem()
uint8_t save_time_and_rfid(uint8_t carrying_rfid) 
*/

#endif /* CHASSI_H_ */