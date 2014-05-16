/*
 * Chassi.c
 *
 * Created: 3/28/2014 3:41:09 PM
 *  Author: johli887
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "Chassi.h"
#include "automatic_steering.h"
#include "engine_control.h"
#include "automatic_steering.h"
#include "../shared/bus.h"
#include "../shared/LCD_interface.h"



void disable_timer_interrupts()
{
	TIMSK0 &= ~(1 << OCIE0A);
}

void enable_timer_interrupts()
{
	TIMSK0 |= (1 << OCIE0A);
}

// ---------------INIT FUNCTIONS-----------------
void timer_interrupt_init()
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

void start_button_init()
{
	PCICR |= (1 << PCIE1); // enable PCINT1
	PCMSK1 |= (1 << PCINT8); // enable Pin 1 on PCINT1
}

//-------------- RFID FUNCTIONS-------------

void read_rfid()
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

//XXX: Not in use?
uint16_t request_RFID_tag()
{
	uint16_t tag;
	//XXX: Callback 6 does not exist in sensor.
	bus_request(BUS_ADDRESS_SENSOR, 6, 0, &tag);
	return tag;
}

void disable_rfid_reader()
{
	uint16_t timeout_counter = 0;
	uint8_t status_code = 1;
	while (timeout_counter < 100 && status_code != 0)
	{
		status_code = bus_transmit(BUS_ADDRESS_SENSOR, 7, 0);
		++timeout_counter;
	}
}

void enable_rfid_reader()
{
	uint16_t timeout_counter = 0;
	uint8_t status_code = 1;
	while (timeout_counter < 100 && status_code != 0)
	{
		status_code = bus_transmit(BUS_ADDRESS_SENSOR, 8, 0);
		++timeout_counter;
	}
	if (status_code != 0)
	{
		////display(0,"TO enable read");
		////display(1,"SC: %d", status_code);
	}
}

void set_sensor_to_linefollowing()
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
	return (uint16_t)steering_wheel;
}


uint8_t is_station(uint8_t station_data)
{
	if (station_data == 0 || station_data == 2)
		return 1;
	else
		return 0;
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

uint8_t station_match_with_carrying(uint8_t current_station)
{
	return (carrying_rfid + 1 == current_station);
}

void request_line_data()
{
	uint8_t timeout_counter = 0;
	uint8_t status_code = 1;
	while (timeout_counter < 100 && status_code != 0)
	{
		status_code = bus_transmit(BUS_ADDRESS_SENSOR, 11, 0);
		++timeout_counter;
	}
	if (status_code != 0)
	{
		//display(0,"TO req line");
		//display(1,"SC: %d", status_code);
	}
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

uint8_t is_pickup_station(uint8_t id)
{
	return (id % 2 == 0);
}

uint8_t skip_station()
{
	uint8_t next_id = 0;
	if (lap_finished == 0)
		return 0;
	else if (station_count < number_of_stations)
	{
		next_id = station_list[station_count + 1];
		++station_count;
	}
	else if (station_count == number_of_stations)
	{
		next_id = station_list[0];
		station_count = 0;
	}
	
	if (carrying_rfid != 0 && !station_match_with_carrying(next_id)) 
	// true if carrying object but no match with next station
	{
		return 1;
	}
	else if (carrying_rfid == 0 && !is_pickup_station(next_id)) 
	// true if not carrying object but next station is not a pickup station
	{
		return 1;
	}
	else
		return 0;
}

void update_station_list(uint8_t station_id)
{
	if(lap_finished == 1)
		return;
	else if (station_list[0] == station_id)
	{
		lap_finished = 1;
		number_of_stations = station_count;
		station_count = 0;
	}
	else
	{
		station_list[station_count] = station_id;
		++station_count;
	}
}

void drive(int8_t curr_error)
{
	pd_update(curr_error);
	//accelerator = STEERING_MAX_SPEED; // Is now an inargument to update_steering instead of global
	steering_wheel = control;
	update_steering(STEERING_MAX_SPEED);
	enable_timer_interrupts();
}

//----------------------------------Timer interrupt (Start of main program)----------------------------------------

ISR(TIMER0_COMPA_vect) // Timer interrupt to update steering
{	
	disable_timer_interrupts();
	request_line_data();
}

//Sensor calls receive_line_data as a response to request_line_data

void receive_line_data(uint8_t id, uint16_t line_data) // Gets called on by Sensor
{
	int8_t curr_error = (uint8_t)(line_data) - 127;
	uint8_t station_data = (uint8_t)(line_data >> 8);
	
	if(PINA & PINA1) {
		manual_control = 1;
	}
	
	if (!is_station(station_data) && (manual_control != 1))	// robot is not on station and linefollowing is on
	{
		drive(curr_error);
	}
	/*
	else if (skip_station()) // returns 1 if ok to skip station
	{
		drive(curr_error);
	}
	*/
	else if (manual_control == 1) // manual control is on.
	{
		stop_wheels();
	}
	else
	{
	stop_wheels();
	read_rfid();
	}
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
		display(0, "rfid found");
		display(1, "id: %u", station_id);
		rfid_to_pc(station_id);
		update_station_list(station_id);
		command_to_arm(station_data, station_id);
	}
	
	else if (station_id == 0 && scan_count < 10) // still no id found, drive forward
	{
		drive_left_wheels(1, 140);
		drive_right_wheels(1, 140);
		++scan_count;
		_delay_ms(5);
		read_rfid();
	}
	else if (station_id == 0 && scan_count < 30) // no id found, start backing
	{
		drive_left_wheels(0, 140);
		drive_right_wheels(0, 140);
		++scan_count;
		_delay_ms(5);
		read_rfid();
	}
	else
	{
		stop_wheels();
		decision_to_pc(6);
		display_station_and_rfid(station_data, 1);
		//drive_to_next();
	}
}

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
	}
	else if(match == 1 && (station_data == 0)) // match with carrying and station to the right
	{
		decision_to_pc(2);
		put_down_to_arm(1);	// 1 = put down object to the right
	//arm_is_done(0,1); // XXX for debugging, skipping waiting for arm
	}
	else if(match == 1 && (station_data == 2)) // match with carrying and station to the left
	{
		decision_to_pc(3);
		put_down_to_arm(0); // 0 = put down object to the left
	//arm_is_done(0,1); // XXX for debugging, skipping waiting for arm
	
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
	//arm_is_done(0,0); // XXX for debugging, skipping waiting for arm
	}
	else if (station_data == 2 && pickup_station == 1) // Not carrying and pickup station left
	{
		carrying_rfid = station_tag;
		decision_to_pc(1);
		pickup_to_arm(0); // 0 = pick up to the left
	//arm_is_done(0,0); // XXX for debugging, skipping waiting for arm
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
	if (pickup_data == 0) // Arm picked up object
	{
		decision_to_pc(7);
		//_delay_ms(2000);
		drive_to_next();
	}
	else if (pickup_data == 1) // Arm put down object
	{
		handled_stations_list[handled_count++] = carrying_rfid;
		handled_stations_list[handled_count++] = carrying_rfid + 1;
		carrying_rfid = 0;
		decision_to_pc(8);
	//	_delay_ms(2000);
		drive_to_next();
	}
	else if (pickup_data == 2) // Arm did not find object to pick up
	{
		decision_to_pc(9);
		carrying_rfid = 0;
		display(0, "arm found");
		display(1, "nothing");
	}
	else 
	{
		decision_to_pc(10); // 10 = unkown error
		display(0, "error");
		display(1, "arm_is_done");
	}
}

void drive_to_next()
{
	scan_count = 0; // reset the scan counter
	set_sensor_to_linefollowing();
	_delay_ms(50);
	enable_timer_interrupts();
}



//--------Pin Change Interrupt start-button----
ISR(PCINT1_vect)
{
	if (PINB & 1)
	{
	disable_timer_interrupts();
	display(0, "start button");
	display(1, "pressed");
	drive_to_next();
	}
}


int main(void)
{

	wdt_disable();
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
	
	bus_register_receive(8, engine_control_command);
	bus_register_receive(11, engine_set_kp);
	bus_register_receive(12, engine_set_kd);
	bus_register_receive(2, arm_is_done);
	bus_register_receive(4, RFID_done);
	bus_register_receive(1, receive_line_data);
	bus_register_receive(0, emergency_stop);
	bus_register_response(5, got_steering_request);
	
	sei();
	start_button_init();
	timer_interrupt_init();

    while(1)
    {

    }
}

void emergency_stop( uint8_t id, uint16_t status) {
	accelerator = 0;
	wdt_enable(WDTO_1S);
	for(;;){}
}
