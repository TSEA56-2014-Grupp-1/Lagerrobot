﻿/*
 * Chassi.h
 *
 * Created: 3/31/2014 1:38:34 PM
 *  Author: johli887
 */


#ifndef CHASSI_H_
#define CHASSI_H_

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

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




uint16_t request_line_data(void);


uint8_t is_station(uint8_t station_data);


void clear_sensor(void);
uint16_t request_rfid_tag(void);
void arm_is_done(uint8_t id, uint16_t pickup_data);
void send_to_arm(uint16_t arm_action);
void display_station_and_rfid(uint8_t station_data, uint8_t tag);
void command_to_arm(uint8_t station_data, uint8_t station_tag);
void RFID_done(uint8_t id, uint16_t id_and_station);
void read_rfid(void);
void timer_interrupt_init(void);
void start_button_init(void);
void disable_timer_interrupts(void);
void enable_timer_interrupts(void);;

void drive_to_next(void);
uint8_t station_is_handled(uint8_t tag);
void decision_to_pc(uint8_t decision);
void rfid_to_pc(uint8_t tag_id);
uint8_t is_pickup_station(uint8_t id);
uint8_t skip_station(void);
void update_station_list(uint8_t station_id);
void drive(int8_t curr_error);
uint8_t is_mission_complete(void);
void clear_station_list(void);

void pickup_to_arm(uint16_t arm_action_trans);
void put_down_to_arm(uint16_t arm_action_trans);


#endif /* CHASSI_H_ */
