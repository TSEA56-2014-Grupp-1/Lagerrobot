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



uint16_t request_line_data()
{
	uint16_t data;
	bus_request(BUS_ADDRESS_SENSOR, 4, 0, &data);
	return data;
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
	bus_transmit(BUS_ADDRESS_SENSOR, 8, 0);
}

void send_to_arm(uint16_t arm_action_trans)
{
	bus_transmit(BUS_ADDRESS_ARM, 1, arm_action_trans);
}

void arm_is_done(uint8_t id, uint16_t pickup_data)
{
	if (pickup_data == 0)
	carrying_rfid = station_list[station_count];
	else
	carrying_rfid = 0;
	//Benefit turn around?
	TIMSK0 |= (1 << OCIE0A); // Enable timer interrupts
}

uint16_t got_steering_request(uint8_t id, uint16_t metadata) {
	return steering_wheel;
}


uint8_t is_station(uint8_t station_data)
{
	if (station_data == 0 || station_data == 2)
	return 1;
	else
	return 0;
}

uint8_t no_line(uint8_t station_data)
{
	return (station_data == 4);
}

uint8_t is_lap_finished(uint8_t station_tag)
{
	uint8_t match_counter = 0;
	for(uint8_t i = 0; i <= station_count; ++i)
	{
		if (station_tag == station_list[i])
		++match_counter;
	}
	if (match_counter < 2)
	return 0;
	else return 1;		
}

uint8_t station_match_with_carrying(uint8_t current_station)
{
	return (carrying_rfid == current_station);
}




//---Timer interrupt----
ISR(TIMER0_COMPA_vect) // Timer interrupt to update steering
{	
	uint16_t line_data = request_line_data(); //Collect line data from sensor unit
	int8_t curr_error = (uint8_t)(line_data) - 127;
	uint8_t station_data = (uint8_t)(line_data >> 8);
	
	if(PINA & PINA1) {
		manual_control = 1;
	}
	
	if (!is_station(station_data) && (manual_control != 1))	// robot is not on station and linefollowing is on
	{
		pd_update(curr_error);
		accelerator = STEERING_MAX_SPEED;
		steering_wheel = control;
		update_steering();
		return;
	}
	else if (!is_station(station_data) && (manual_control == 1)) // robot is not on station and linefollowing is off 
	{
		
		return;
	}
	
	stop_wheels();
	TIMSK0 = (TIMSK0 & (0 << OCIE0A)); // Disable timer-interrupt since waiting for Arm!  reg TIMSK0 bit OCIE0A = 0
}

int main(void)
{
	bus_init(1); // 1 = BUS_ADDRESS_CHASSIS
	_delay_ms(100);
	engine_init();
	regulator_init();
	
	carrying_rfid = 0;
	first_lap_done = 0;
	station_count = 0;
	manual_control = 0;
	
	bus_register_receive(8, engine_control_command);
	bus_register_receive(11, engine_set_kp);
	bus_register_receive(12, engine_set_kd);
	bus_register_receive(2, arm_is_done);
	bus_register_response(5, got_steering_request);
	sei();
	//enable timer interrupts for ocie0a
 	TIMSK0 |= (1 << OCIE0A);
 	TIFR0 |= (1 << OCF0A);
// 	
	// interrupt frequency 30hz --- or 60hz according to bus-reads with OCR0A set to 0xFF?? 0x80 --> double compared to 0xFF
	OCR0A = 0x80; 
	
	// set to mode 2 (CTC) => clear TCNT0 on compare match
	TCCR0A |= (1 << WGM01 | 0 << WGM00); 
	TCCR0B |= (0 << WGM02);

	//prescale
	TCCR0B |= (1 << CS02 | 0 << CS01 | 1 << CS00);
	
    while(1)
    {

    }
}