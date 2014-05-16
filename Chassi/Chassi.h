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


// Global variables
uint8_t carrying_rfid;
uint8_t handled_stations_list[18];
uint8_t handled_count;
uint8_t follow_line;
uint8_t	manual_control;
uint8_t scan_count;
uint8_t station_count;
uint8_t station_list[18];
uint8_t lap_finished;
uint8_t number_of_stations;



/**
 * @brief Requests line data from the sensor unit.
 * @details Performs a bus request to the sensor unit for data on the center
 * of mass and station information.
 * @return The data returned by the Sensor unit. High byte contains station information, 
 * low byte contains the value of the center of mass for the line sensor.
 */

/**
 * @brief Checks if provided station information indicates a station.
 * 
 * @param station_data The station information to be checked. 0 or 2 indicates a station.
 * @return 1 if the information indicates a station, 0 otherwise.
 */
uint8_t is_station(uint8_t station_data);
void clear_sensor();
uint16_t request_rfid_tag();
void arm_is_done(uint8_t id, uint16_t pickup_data);
void send_to_arm(uint16_t arm_action);
void display_station_and_rfid(uint8_t station_data, uint8_t tag);
void command_to_arm(uint8_t station_data, uint8_t station_tag);
void RFID_done(uint8_t id, uint16_t id_and_station);
void read_rfid();
void timer_interrupt_init();
void start_button_init();
void disable_timer_interrupts();
void enable_timer_interrupts();
void receive_line_data(uint8_t id, uint16_t line_data);
void request_line_data();
void drive_to_next();
uint8_t station_is_handled(uint8_t tag);
void decision_to_pc(uint8_t decision);
void rfid_to_pc(uint8_t tag_id);
uint8_t is_pickup_station(uint8_t id);
uint8_t skip_station();
void update_station_list(uint8_t station_id);
void drive(int8_t curr_error);
uint8_t is_mission_complete();
void clear_station_list();

void pickup_to_arm(uint16_t arm_action_trans);
void put_down_to_arm(uint16_t arm_action_trans);

void emergency_stop(uint8_t id, uint16_t metadata);

uint16_t got_steering_request(uint8_t id, uint16_t metadata);

#endif /* CHASSI_H_ */