/**
 *	@file mission_control.c
 *	@author Andreas Runfalk
 *
 *	Control functions for remembering station order and which ones are handled.
 */

#include <avr/io.h>
#include "mission_control.h"


/**
 *	List of all stations where the first element is the first added station
 */
uint8_t station_list[MC_STATION_MAX];

/**
 *	Will contain 0 for each unhandled index and 1 for handled indexes
 */
uint8_t station_list_handled[MC_STATION_MAX];

/**
 *	Number of elements currently in station list
 */
uint8_t station_list_write_index = 0;

/**
 *	Remember if all stations are discovered or not
 */
uint8_t station_cycled = 0;


/**
* Check if robot is on a station
*/
uint8_t mc_is_station(uint8_t station_data)
{
	return station_data == 0 || station_data == 2;
}

/**
* Check if station matches with what robot is carrying
*/
uint8_t mc_station_match_with_carrying(uint8_t current_station, uint8_t carrying_rfid)
{
	return (carrying_rfid + 1 == current_station);
}

/**
* Finds the next pickup station from where robot is now. Possible status codes are:
*
*	- 0 if the next pickup stations id was found
*	- 1 if not
*
*/
uint8_t mc_find_next_pickup(uint8_t station_id, uint8_t *next_pickup_id) {
	uint8_t station_index_a = mc_find_station(station_id);
	uint8_t id = 0;
	uint8_t i;
	for (i = station_index_a + 1; i != station_index_a; ++i)
	{
		mc_get_station_id(i, &id);
		if (mc_is_pickup_station(id) && !mc_is_handled(id)) {
			*next_pickup_id = id;
			return 0;
		}
		else if (i >= MC_STATION_MAX)
			i = 0;
	}
	return 1;	
}


/**
* Check if station is a pickup station
*/
uint8_t mc_is_pickup_station(uint8_t station_id){
		return (station_id % 2 == 0);
}


/**
 *	Clear all known data about stations
 */
void mc_clear() {
	// Clear station order
	station_list_write_index = 0;

	// Mark all stations unhandled
	uint8_t i;
	for (i = 0; i < MC_STATION_MAX; i++) {
		station_list_handled[i] = 0;
	}

	// Mark lap uncomplete
	station_cycled = 0;
}

/**
 *	Number of found stations
 */
uint8_t mc_station_count() {
	return station_list_write_index;
}

/**
 *	Add station to list if not already there. Possible status codes are:
 *
 *	- 0 if added or mark of loop
 *	- 1 if all stations are already known
 *	- 2 if stations are scrambled
 */
uint8_t mc_add_station(uint8_t station_id) {
	// Check if all stations are already known
	if (mc_is_cycled()) {
		return 1;
	}

	// If station is already known this is probably the end of the loop
	if (mc_has_station(station_id)) {
			uint8_t first_station = 0;
			mc_get_station_id(0, &first_station);
		if (first_station == station_id) {
			station_cycled = 1;
			return 0;
		} else {
			// Station was not the one we first discovered yet we already know it,
			// hence order has been scrambled
			return 2;
		}
	} else {
		station_list[station_list_write_index++] = station_id;
		return 0;
	}
}

/**
 *	Retrieve station at given index
 *
 *	@return 0 if station index was known, else 1
 */
uint8_t mc_get_station_id(uint8_t station_index, uint8_t *station_id) {
	if (station_index < station_list_write_index) {
		*station_id = station_list[station_index];
		return 0;
	} else {
		return 1;
	}
}

/**
 *	Determine if a station is known or not
 *
 *	@return 1 if station is known, else 0
 */
uint8_t mc_has_station(uint8_t station_id) {
	return mc_find_station(station_id) != -1;
}

/**
 *	Find index of the given station ID
 *
 *	@return 0-bound index if known station else -1
 */
int8_t mc_find_station(uint8_t station_id) {
	uint8_t i;
	for (i = 0; i < station_list_write_index; i++) {
		if (station_list[i] == station_id) {
			return i;
		}
	}
	return -1;
}

/**
 *	Checks if all stations have been visited at least once
 *
 *	@return 0 if there may still be undiscovered stations, else 1
 */
uint8_t mc_is_cycled() {
	return station_cycled;
}

/**
 *	Check if station is marked as handled
 */
uint8_t mc_is_handled(uint8_t station_id) {
	int8_t station_index = mc_find_station(station_id);

	if (station_index == -1) {
		return 0;
	} else {
		return station_list_handled[station_index];
	}
}

/**
 *	Handle station. Possible status codes are:
 *
 *	- 0 if station is handled successfully
 *	- 1 if station is unknown
 *	- 2 if station was already handled
 *
 *	@return Status code
 */
uint8_t mc_handle_station(uint8_t station_id) {
	int8_t station_index = mc_find_station(station_id);

	if (station_index == -1) {
		return 1;
	}

	if (station_list_handled[station_index]) {
		return 2;
	}

	station_list_handled[station_index] = 1;
	return 0;
}

/**
 *	Check if there are at least one station and that they are all handled
 */
uint8_t mc_all_handled() {
	if (station_list_write_index == 0 || !mc_is_cycled()) {
		return 0;
	}

	uint8_t i;
	for (i = 0; i < station_list_write_index; i++) {
		if (!station_list_handled[i]) {
			return 0;
		}
	}

	return 1;
}

/**
 *	Calculate number of stations from A to B where A is the current station,
 *	assuming robot is moving forward from A to B. If A is index 10 and B is index
 *	12 distance will be 2.
 */
uint8_t mc_find_distance(uint8_t station_id_a, uint8_t station_id_b, uint8_t *distance) {
	int8_t station_index_a = mc_find_station(station_id_a);
	int8_t station_index_b = mc_find_station(station_id_b);

	if (station_index_a == -1) {
		return 1;
	}

	if (station_index_b == -1) {
		return 2;
	}

	if (station_index_a == station_index_b) {
		return 3;
	}

	// If B has lower index than A, increase it by number of elements so B is always
	if (station_index_b < station_index_a) {
		// If we haven't yet reached a full cycle the answer is not known
		if (!station_cycled) {
			return 4;
		}
		station_index_b += station_list_write_index;
	}

	*distance = station_index_b - station_index_a;
	return 0;
}
