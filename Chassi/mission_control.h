#ifndef MISSION_CONTROL_H_
#define MISSION_CONTROL_H_
/**
 *	@file mission_control.h
 *	@author Andreas Runfalk
 *
 *	Control functions for remembering station order and which ones are handled.
 */

/**
 *	Maximum number of stations that can be detected
 */
#define MC_STATION_MAX 18

void mc_clear();
uint8_t mc_station_count();
uint8_t mc_station_match_with_carrying(uint8_t current_station, uint8_t carrying_rfid);
uint8_t mc_add_station(uint8_t station_id);
uint8_t mc_get_station_id(uint8_t station_index, uint8_t *station_id);
uint8_t mc_has_station(uint8_t station_id);
int8_t mc_find_station(uint8_t station_id);
uint8_t mc_is_cycled();
uint8_t mc_is_handled(uint8_t station_id);
uint8_t mc_handle_station(uint8_t station_id);
uint8_t mc_all_handled();
uint8_t mc_find_distance(uint8_t station_id_a, uint8_t station_id_b, uint8_t *distance);
uint8_t mc_is_pickup_station(uint8_t station_id);
uint8_t mc_find_next_pickup(uint8_t station_id, uint8_t *next_pickup_id);
uint8_t mc_is_station(uint8_t station_data);

#endif /* MISSION_CONTROL_H_ */
