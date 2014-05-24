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
#include "mission_control.h"

uint8_t distance = 0;
uint16_t counter = 0;


void disable_timer_interrupts(void)
{
	TIMSK0 &= ~(1 << OCIE0A);
}

void enable_timer_interrupts(void)
{
	TIMSK0 |= (1 << OCIE0A);
}

// ---------------INIT FUNCTIONS-----------------
void timer_interrupt_init(void)
{
	//enable timer interrupts for ocie0a
	//TIMSK0 |= (1 << OCIE0A);
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

uint16_t got_steering_request(uint8_t id, uint16_t metadata) {
	return get_steering_wheel();
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

void display_finished() {
	display(0, "Mission");
	display(1, "Complete!!");
}

uint8_t station_match_with_carrying(uint8_t current_station)
{
	return (carrying_rfid + 1 == current_station);
}

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



void drive(int8_t curr_error)
{
	// Update steering with new PD value
	update_steering(pd_update(curr_error), STEERING_MAX_SPEED);
	enable_timer_interrupts();
}

//----------------------------------Timer interrupt (Start of main program)----------------------------------------

ISR(TIMER0_COMPA_vect) // Timer interrupt to update steering
{
	disable_timer_interrupts();
	uint16_t line_data = 0;
	line_data = request_line_data();
	int8_t curr_error = (uint8_t)(line_data) - 127;
	uint8_t station_data = (uint8_t)(line_data >> 8);
	// Determine if we're in manual mode or not
	manual_control = PINA & PINA1;

	if (!mc_is_station(station_data) && manual_control != 1)
	{
		// Robot is not on station and linefollowing is on
		drive(curr_error);
	}
	else if (distance > 1)
	{
		// Just continue if station should be skipped
		--distance;
		drive(curr_error);
	}
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
	if (status_code != 0)
	{
		//display(0,"TO req line");
		//display(1,"SC: %d", status_code);
	}

	return data;
}

// Sensor calls RFID_done as a response to read_rfid
void RFID_done(uint8_t id, uint16_t id_and_station)
{
	uint8_t station_id = (uint8_t)(id_and_station);
	uint8_t station_data = (uint8_t)(id_and_station >> 8);
	if (station_id != 0) // rfid found!
	{
		stop_wheels();
		display_station_and_rfid(station_data, station_id);
		rfid_to_pc(station_id);
		//update_station_list(station_id);
		mc_add_station(station_id);
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
		mc_clear(); // Should this be done everytime?
		drive_to_next();
	}
}

void command_to_arm(uint8_t station_data, uint8_t station_tag)
{
	uint8_t match = mc_station_match_with_carrying(station_tag, carrying_rfid);
	uint8_t pickup_station = mc_is_pickup_station(station_tag);

	if (mc_all_handled()) {
		display_finished();
	}
	//if (station_is_handled(station_tag))
	else if (mc_is_handled(station_tag))
	{
		display(0, "st allready");
		display(1, "handled");
		decision_to_pc(5);
		drive_to_next();
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

// Arm calls to arm_is_done as a response to send_to_arm
void arm_is_done(uint8_t id, uint16_t pickup_data)
{
	switch (pickup_data) {
		case 0: // Arm picked up object
			decision_to_pc(7);
			mc_find_distance(carrying_rfid, carrying_rfid + 1, &distance);
			drive_to_next();
			break;
		case 1: // Arm put down an object
			// Mark station as handled
			mc_handle_station(carrying_rfid);
			mc_handle_station(++carrying_rfid);
			decision_to_pc(8);
			
			uint8_t	next_pickup_id = 0;
			// Check if all stations are handled
			if (mc_all_handled())
				display_finished();		
			else if (!mc_find_next_pickup(carrying_rfid, &next_pickup_id))
				mc_find_distance(carrying_rfid, next_pickup_id, &distance);		
			else 
			{
				carrying_rfid = 0;
				drive_to_next();
			}
			break;
		case 2: // Sensor did not find an object to pick up
			decision_to_pc(9);
			display(0, "sensor found");
			display(1, "nothing");
			break;
		case 3: // Sensor found object but arm failed to pick it up
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
 *	Start and stop line following
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
 *	Initiate linefollowing to the next station
 */
void drive_to_next(void)
{
	set_sensor_to_linefollowing();
	scan_count = 0;
	_delay_ms(100);
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
	bus_register_response(5, got_steering_request);
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
