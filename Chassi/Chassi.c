/*
 * Chassi.c
 *
 * Created: 3/28/2014 3:41:09 PM
 *  Author: johli887
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Chassi.h"
#include "automatic_steering.h"
#include "engine_control.h"
#include "automatic_steering.h"
#include "../shared/bus.h"
#include "../shared/LCD_interface.h"
#include "../shared/packets.h"
uint16_t counter = 0;

/**
 *	To stop asking for line data
 */
void disable_timer_interrupts(void)
{
	TIMSK0 &= ~(1 << OCIE0A);
}

/**
 *	To stat asking for line data
 */
void enable_timer_interrupts(void)
{
	TIMSK0 |= (1 << OCIE0A);
}

// ---------------INIT FUNCTIONS-----------------
void timer_interrupt_init(void)
{
	//enable timer interrupts for ocie0a
	TIFR0 |= (1 << OCF0A);

	// interrupt frequency 30hz --- or 60hz according to bus-reads with OCR0A set to 0xFF?? 0x80 --> double compared to 0xFF
	OCR0A = 0x80;

	// set to mode 2 (CTC) => clear TCNT0 on compare match
	TCCR0A |= (1 << WGM01 | 0 << WGM00);
	TCCR0B |= (0 << WGM02);
	//prescale
	TCCR0B |= (1 << CS02 | 0 << CS01 | 1 << CS00);
}

void start_button_init(void)
{
	PCICR |= (1 << PCIE1); // enable PCINT1
	PCMSK1 |= (1 << PCINT8); // enable Pin 1 on PCINT1
}

//-------------- RFID FUNCTIONS-------------
/**
 *	In order to identify station
 */
void read_rfid(void)
{
	uint16_t timeout_counter = 0;
	uint8_t status_code = 1;
	while(timeout_counter < 100 && status_code != 0)
	{
		status_code = bus_transmit(BUS_ADDRESS_SENSOR, 10, 0);
		++ timeout_counter;
	}
	if (status_code != 0)
	{
		////display(0,"TO read rfid");
		////display(1,"SC: %d", status_code);
	}
}

/**
 * Change sensor task so it's set correct when asking for line data
 */
void set_sensor_to_linefollowing(void)
{
	uint16_t timeout_counter = 0;
	uint8_t status_code = 1;
	while(timeout_counter < 1000 && status_code != 0)
	{
		status_code = bus_transmit(BUS_ADDRESS_SENSOR, 9, 0);
		++ timeout_counter;
	}
	if (status_code != 0)
	{
		//display(0,"TO clear");
		//display(1,"SC: %d", status_code);
	}
}

/**
 * To pickup object
 *
 *	@param arm_action_trans Which side object is on
 */
void pickup_to_arm(uint16_t arm_action_trans)
{

		uint16_t timeout_counter = 0;
		uint8_t status_code = 1;
		while (timeout_counter < 100 && status_code != 0)
		{
			status_code = bus_transmit(BUS_ADDRESS_ARM, 2, arm_action_trans);
			++timeout_counter;
		}
		if (status_code != 0)
		{
			//display(0,"TO to arm"); // TO = TimeOut
			//display(1,"SC: %d", status_code); // SC = Status Code
		}
}

/**
 * To put down objects
 *
 *	@param arm_action_trans Which side to put down
 */
void put_down_to_arm(uint16_t arm_action_trans)
{

	uint16_t timeout_counter = 0;
	uint8_t status_code = 1;
	while (timeout_counter < 100 && status_code != 0)
	{
		status_code = bus_transmit(BUS_ADDRESS_ARM, 12, arm_action_trans);
		++timeout_counter;
	}
	if (status_code != 0)
	{
		//display(0,"TO to arm"); // TO = TimeOut
		//display(1,"SC: %d", status_code); // SC = Status Code
	}
}


/**
 * @brief Checks if provided station information indicates a station.
 *
 * @param station_data The station information to be checked. 0 or 2 indicates a station.
 * @return 1 if the information indicates a station, 0 otherwise.
 */
uint8_t is_station(uint8_t station_data)
{
	return station_data == 0 || station_data == 2;
}


void display_station_and_rfid(uint8_t station_data, uint8_t tag)
{
	if (station_data == 2)
	{
		display(0, "left");
		display(1, "rfid: %d", tag);
	}
	else if (station_data == 0)
	{
		display(0, "right");
		display(1, "rfid: %d", tag);
	}
	else if (station_data == 1)
	{
		display(0, "no station");
		display(1, "rfid: %d", tag);
	}
	else
	{
		display(0, "unkown");
		display(1, "error");
	}
}

/**
 * To know if station is handled
 *
 * @param tag The tag sensor has read on station
 * @return 1 if station is allready handled, 0 otherwise
 */
uint8_t station_is_handled(uint8_t tag)
{
	uint8_t i;
	for(i = 0; i <= 18; ++i)
	{
		if(handled_stations_list[i] == tag)
			return 1;
	}
	return 0;
}

/**
 * To know if it's the station to put down objects on
 *
 * @param current_station The station robot has stopped on
 * @return 1 if current_station is next after carrying_rfid
 */
uint8_t station_match_with_carrying(uint8_t current_station)
{
	return (carrying_rfid + 1 == current_station);
}

/**
 * To know what decision robot is taking by looking at PC
 *
 * @param decision as follows:
 * 0    Station to the right
 * 1	Station to the left
 * 2	Putting down object to the right
 * 3	Putting down object to the left
 * 4	ID did not match object
 * 5	Station is already handeld
 * 6	No ID was found
 * 7	Arm has picked up an object
 * 8	Arm has put down the object
 * 9	Object was not found by arm
 * 10	An unkown error has occured
 * 11   Arm failed pickup
 * 12	Started linefollowing
 */
void decision_to_pc(uint8_t decision)
{
	uint8_t timeout_counter = 0;
	uint8_t status_code = 1;
	while (timeout_counter < 100 && status_code != 0)
	{
		status_code = bus_transmit(BUS_ADDRESS_COMMUNICATION, 8, decision);
		++timeout_counter;
	}

}

/**
 * To know what id robot has read
 *
 * @param tag_id The stations id
 */
void rfid_to_pc(uint8_t tag_id)
{
	uint8_t timeout_counter = 0;
	uint8_t status_code = 1;
	while (timeout_counter < 100 && status_code != 0)
	{
		status_code = bus_transmit(BUS_ADDRESS_COMMUNICATION, 9, tag_id);
		++timeout_counter;
	}
}

/**
 * To know if it's a pickúp station or put down station
 *
 * @param id The station robot has stopped on
 * @return 1 if id is a pickup station
 */
uint8_t is_pickup_station(uint8_t id)
{
	return (id % 2 == 0);
}

/**
 * To keep track of what stations robot has visited
 *
 * @param station_id The station robot has stopped on
 */
void update_station_list(uint8_t station_id)
{
	if(lap_finished == 1)
		return;
	else if (station_list[0] == station_id)
	{
		lap_finished = 1;
		number_of_stations = station_count;
		station_count = 1;
	}
	else
	{
		station_list[station_count++] = station_id;
	}
}

/**
 * To be able to follow the line
 *
 * @param curr_error The error from the line
 */
void drive(int8_t curr_error)
{
	// Update steering with new PD value
	update_steering(pd_update(curr_error), STEERING_MAX_SPEED);
	enable_timer_interrupts();
}

/**
 * If the order has been scrambled or robot failed to read rfid
 */
void clear_station_list(void)
{
	uint8_t i = 0;
	for (i = 0; i < 18; ++i)
	{
		station_list[i] = 0;
	}
	station_count = 0;
	
}

//----------------------------------Timer interrupt (Start of main program)----------------------------------------
/**
 * Update the linefollowing or stop at station
 */
ISR(TIMER0_COMPA_vect) // Timer interrupt to update steering
{
	disable_timer_interrupts();
	uint16_t line_data = 0;
	//line_data = 0x100 + 127;
	line_data = request_line_data();
	int8_t curr_error = (uint8_t)(line_data) - 127;
	uint8_t station_data = (uint8_t)(line_data >> 8);
	/*
	if (++counter >= 100)
	{
		display(0, "%s", curr_error);
		counter = 0;
	}
	*/
	// Determine if we're in manual mode or not
	manual_control = PINA & PINA1;

	if (!is_station(station_data) && manual_control != 1)
	{
		// Robot is not on station and linefollowing is on
		drive(curr_error);
	}
	/*
	else if (skip_station())
	{
		// Just continue if station should be skipped
		drive(curr_error);
	}
	*/
	else if (manual_control == 1)
	{
		// Stop wheels when in manual control mode
		stop_wheels();
	}
	else
	{
		// New unknown station
		stop_wheels();
		read_rfid();
	}
}

/**
 * @brief Requests line data from the sensor unit.
 * @details Performs a bus request to the sensor unit for data on the center
 * of mass and station information.
 * @return The data returned by the Sensor unit. High byte contains station information,
 * low byte contains the value of the center of mass for the line sensor.
 */
uint16_t request_line_data(void)
{
	uint8_t timeout_counter = 0;
	uint8_t status_code = 1;
	uint16_t data = 0;
	while (timeout_counter < 100 && status_code != 0)
	{
		status_code = bus_request(BUS_ADDRESS_SENSOR, 11, 0, &data);
		++timeout_counter;
	}
	return data;
}

/**
 * Sensor calls RFID_done as a response to read_rfid to take action
 *
 * @param id_and_station The station robot has stopped on and which side it is on
 */
void RFID_done(uint8_t id, uint16_t id_and_station)
{
	uint8_t station_id = (uint8_t)(id_and_station);
	uint8_t station_data = (uint8_t)(id_and_station >> 8);
	if (station_id != 0) // rfid found!
	{
		stop_wheels();
		display_station_and_rfid(station_data, station_id);
		rfid_to_pc(station_id);
		update_station_list(station_id);
		command_to_arm(station_data, station_id);
	}
	else if (station_id == 0 && scan_count < 18) // No id found, drive forward
	{
		drive_left_wheels(1, 130);
		drive_right_wheels(1, 130);
		++scan_count;
		_delay_ms(5);
		read_rfid();
	}
	else if (station_id == 0 && scan_count < 35) // Still no id found, start backing
	{
		drive_left_wheels(0, 130);
		drive_right_wheels(0, 130);
		++scan_count;
		_delay_ms(5);
		read_rfid();
	}
	else
	{
		stop_wheels();
		decision_to_pc(6);
		display(0, "No tag ");
		display(1, "found!");
		//scan_count = 0;
		if (lap_finished == 0)
			clear_station_list();
		drive_to_next();
	}
}

/**
 * Decide what decision to be sent to arm
 * 
 * @param station_data What side station is on
 * @param station_tag The id of the station the robot is on
 */
void command_to_arm(uint8_t station_data, uint8_t station_tag)
{
	uint8_t match = station_match_with_carrying(station_tag);
	uint8_t pickup_station = is_pickup_station(station_tag);

	if (station_is_handled(station_tag))
	{
		display(0, "st allready");
		display(1, "handled");
		decision_to_pc(5);
		drive_to_next();
		if (!is_mission_complete()) {
			drive_to_next();
		}
	}
	else if(match == 1 && (station_data == 0)) // match with carrying and station to the right
	{
		decision_to_pc(2);
		put_down_to_arm(1);	// 1 = put down object to the right
	}
	else if(match == 1 && (station_data == 2)) // match with carrying and station to the left
	{
		decision_to_pc(3);
		put_down_to_arm(0); // 0 = put down object to the left
	}
	else if (carrying_rfid != 0 || pickup_station == 0) // Carrying object or not a pickupstation
	{
		decision_to_pc(4);
		drive_to_next();
	}
	else if (station_data == 0 && pickup_station == 1) // Not carrying and pickup station right
	{
		carrying_rfid = station_tag;
		decision_to_pc(0);
		pickup_to_arm(1); // 1 = pick up to the right
	}
	else if (station_data == 2 && pickup_station == 1) // Not carrying and pickup station left
	{
		carrying_rfid = station_tag;
		decision_to_pc(1);
		pickup_to_arm(0); // 0 = pick up to the left
	}
	else
	{
		decision_to_pc(10); // 10 = unkown error
		display(0, "error");
		display(1, "cmd to arm");
	}
}


/**
 * Arm calls to arm_is_done as a response to send_to_arm
 *
 * @param id The station robot has stopped on
 *
 * @param pickup_data A value for what arm did
 * 0 = Arm picked up object
 * 1 = Arm put down an object
 * 2 = Arm did not find an object to pick up
 * 3 = Arm found object but failed to pick it up
 */
void arm_is_done(uint8_t id, uint16_t pickup_data)
{
	switch (pickup_data) {
		case 0: // Arm picked up object
			decision_to_pc(7);
			drive_to_next();
			break;
		case 1: // Arm put down an object
			// Mark station as handled
			handled_stations_list[handled_count++] = carrying_rfid;
			handled_stations_list[handled_count++] = carrying_rfid + 1;

			carrying_rfid = 0;
			decision_to_pc(8);

			// Check if all stations are handled
			if (!is_mission_complete()) {
				drive_to_next();
			}
			break;
		case 2: // Arm did not find an object to pick up
			decision_to_pc(9);
			carrying_rfid = 0;

			display(0, "arm found");
			display(1, "nothing");
			break;
		case 3: // Arm found object but failed to pick it up
			decision_to_pc(11);

			display(0, "failed to");
			display(1, "pick up");
			break;
		default:
			decision_to_pc(10); // 10 = unkown error

			display(0, "error");
			display(1, "arm_is_done");
			break;
	}
}

/**
 *	Start and stop line following from the computer
 *
 *@param data:
 * 1 = follow line
 *
 *0 = stop following line
 */
void control_line_following(uint8_t id, uint16_t data) {
	if (data) {
		drive_to_next();
	} else {
		disable_timer_interrupts();
		stop_wheels();
	}
}

/**
 *	To know if robot is done with all the stations
 * @return 1 if mission is complete
 */
uint8_t is_mission_complete(void)
{
	if (lap_finished == 0)
		return 0;
	else if(handled_count == number_of_stations)
	{
		display(0, "Mission");
		display(1, "Complete!!");
		return 1;
	}
	else
		return 0;
}

/**
 *	Initiate linefollowing to the next station
 */
void drive_to_next(void)
{
	set_sensor_to_linefollowing();
	scan_count = 0;
	_delay_ms(50);
	enable_timer_interrupts();
}

/**
 *	Handle start button press
 */
ISR(PCINT1_vect)
{
	if (PINB & 1) {
		disable_timer_interrupts();
		decision_to_pc(DEC_START_LINE);
		display(0, "Mission");
		display(1, "Start");
		drive_to_next();
	}
}


int main(void)
{

	bus_init(BUS_ADDRESS_CHASSIS);
	_delay_ms(100);
	engine_init();
	regulator_init();

	carrying_rfid = 0;
	handled_count = 0;
	manual_control = 0;
	scan_count = 0;
	lap_finished = 0;
	number_of_stations = 0;
	station_count = 0;

	// bus_register_receive(1, receive_line_data);
	bus_register_receive(2, arm_is_done);
	bus_register_receive(3, control_line_following);
	bus_register_receive(4, RFID_done);
	bus_register_receive(8, engine_control_command);
	bus_register_receive(11, engine_set_kp);
	bus_register_receive(12, engine_set_kd);

	sei();
	start_button_init();
	timer_interrupt_init();

    while(1)
    {

    }
}
