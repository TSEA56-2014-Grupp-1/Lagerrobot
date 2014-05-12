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

void clear_sensor()
{
	uint16_t timeout_counter = 0;
	uint8_t status_code = 1;
	while(timeout_counter < 100 && status_code != 0) 
	{
		status_code = bus_transmit(BUS_ADDRESS_SENSOR, 9, 0);
		++ timeout_counter;
	}
	if (status_code != 0)
	{
		display(0,"TO clear");
		display(1,"SC: %d", status_code);
	}
}

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




uint16_t request_RFID_tag()
{
	uint16_t tag;
	bus_request(BUS_ADDRESS_SENSOR, 6, 0, &tag);
	return tag;
}

void disable_rfid_reader()
{
	bus_transmit(BUS_ADDRESS_SENSOR, 7, 0);
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
		display(0,"TO enable read");
		display(1,"SC: %d", status_code);
	}
}

void send_to_arm(uint16_t arm_action_trans)
{
	
		uint16_t timeout_counter = 0;
		uint8_t status_code = 1;
		while (timeout_counter < 100 && status_code != 0)
		{
			status_code = bus_transmit(BUS_ADDRESS_ARM, 1, arm_action_trans);
			++timeout_counter;
		}
		if (status_code != 0)
		{
			display(0,"TO enable read"); // TO = TimeOut
			display(1,"SC: %d", status_code); // SC = Status Code
		}
}


uint8_t is_station(uint8_t station_data)
{
	if (station_data == 0 || station_data == 2)
	return 1;
	else
	return 0;
}

uint8_t station_match_with_carrying(uint8_t current_station)
{
	return (carrying_rfid == current_station);
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


void send_command_to_arm(uint8_t station_data, uint8_t station_tag)
{
	if(station_match_with_carrying(station_tag) && (station_data == 0)) // carrying and station to the right
	send_to_arm(2);	// 2 = put down object to the right
	else if(station_match_with_carrying(station_tag) && (station_data == 2)) // carrying and station to the left
	send_to_arm(3); // 3 = put down object to the left
	else if (carrying_rfid == 0 && (station_data == 0)) // Not carrying and station right
	{
		carrying_rfid = station_tag;
		send_to_arm(0); // 0 = pick up to the right
	}
	else if (carrying_rfid == 0 && (station_data == 2)) // Not carrying and station left
	{
		carrying_rfid = station_tag;
		send_to_arm(1); // 1 = pick up to the left
	}
	else if (carrying_rfid != 0) // Carrying object and no match with station
	{
		return;
	}
}

void display_command(uint8_t station_tag)
{
	if (station_match_with_carrying(station_tag))
		display(1, "put down");
	else if (carrying_rfid == 0)
		display(1, "pick up");
	else
		display(1, "wrong station");
}



void start_button_init()
{
	PCICR |= (1 << PCIE1); // enable PCINT1
	PCMSK1 |= (1 << PCINT8); // enable Pin 1 on PCINT1
}



void disable_timer_interrupts()
{
	TIMSK0 &= ~(1 << OCIE0A);
}

void enable_timer_interrupts()
{
	TIMSK0 |= (1 << OCIE0A);
}


//----------------Timer interrupt------
ISR(TIMER0_COMPA_vect) // Timer interrupt to update steering
{	
	disable_timer_interrupts();
	request_line_data();
}

void request_line_data()
{
	uint16_t timeout_counter = 0;
	uint8_t status_code = 1;
	while (timeout_counter < 10 && status_code != 0)
	{
		status_code = bus_transmit(BUS_ADDRESS_SENSOR, 11, 0);
		++timeout_counter;
	}
	if (status_code != 0)
	{
		display(0,"TO req line");
		display(1,"SC: %d", status_code);
	}
}

void receive_line_data(uint8_t id, uint16_t line_data)
{
	int8_t curr_error = (uint8_t)(line_data) - 127;
	uint8_t station_data = (uint8_t)(line_data >> 8);
	if(PINA & PINA1) {
		manual_control = 1;
	}
	
	if (!is_station(station_data) && (manual_control != 1))	// robot is not on station and linefollowing is on
	{
		pd_update(curr_error);
		accelerator = STEERING_MAX_SPEED; // XXX Could be an inargument instead of global
		steering_wheel = control;
		update_steering();
		enable_timer_interrupts();
		return;
	}
	else if (!is_station(station_data) && (manual_control == 1)) // robot is not on station and linefollowing is off
	{
		stop_wheels();
		enable_timer_interrupts();
		return;
	}
	stop_wheels();
	_delay_ms(200);
	_delay_ms(200);
	_delay_ms(200);
	_delay_ms(200);
	
	read_rfid();
}

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
		display(0,"TO read rfid");
		display(1,"SC: %d", status_code);
	}
}

//----- RFID is done -----
void RFID_done(uint8_t id, uint16_t id_and_station)
{
	int8_t station_id = (uint8_t)(id_and_station);
	uint8_t station_data = (uint8_t)(id_and_station >> 8);
	if (manual_control != 1 && station_id == 0 && scan_count < 5)
	{
		drive_left_wheels(1, 200);
		drive_right_wheels(1, 200);
		_delay_ms(100);
		stop_wheels();
		_delay_ms(100);
		++scan_count;
		read_rfid();
		return;
	}
	else if (manual_control != 1 && station_id == 0 && scan_count < 30)
	{
		drive_left_wheels(0, 200);
		drive_right_wheels(0, 200);
		_delay_ms(100);
		stop_wheels();	
		_delay_ms(100);
		++scan_count;
		read_rfid();
		return;
	}
	else if (manual_control != 1)
	{
		stop_wheels();
		disable_rfid_reader();
		display_station_and_rfid(station_data, station_id);
	}
	else
	{
		display_station_and_rfid(station_data, station_id);
	}
}


//----- Arm done-----
void arm_is_done(uint8_t id, uint16_t pickup_data)
{
	if (pickup_data == 1) // Arm did put down object
	{
		handled_stations_list[station_count] = carrying_rfid;
		carrying_rfid = 0;
	}
	else if (pickup_data == 2) // Arm did not find object to pick up
		carrying_rfid = 0;
	enable_timer_interrupts();
}

//---Pin Change Interrupt start-button----
ISR(PCINT1_vect)
{
	disable_timer_interrupts();
	display(0, "start button");
	display(1, "pressed");
	scan_count = 0; // reset the scan counter
	enable_rfid_reader();
	_delay_us(20);
	clear_sensor();
	enable_timer_interrupts();
	//return;
}


int main(void)
{
	
	bus_init(1); // 1 = BUS_ADDRESS_CHASSIS
	_delay_ms(100);
	engine_init();
	regulator_init();
	
	carrying_rfid = 0;
	station_count = 0;
	manual_control = 0;
	scan_count = 0;
	
	bus_register_receive(8, engine_control_command);
	bus_register_receive(11, engine_set_kp);
	bus_register_receive(12, engine_set_kd);
	bus_register_receive(2, arm_is_done);
	bus_register_receive(4, RFID_done);
	bus_register_receive(1, receive_line_data);
	
	sei();
	start_button_init();
	_delay_ms(100);
	timer_interrupt_init();


	
    while(1)
    {

    }
}
