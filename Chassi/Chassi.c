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

//---- Functions----
uint16_t request_line_data()
{
	uint16_t data;
	bus_request(4, 4, 0, &data);
	return data;
}

uint8_t is_station(uint8_t station_data)
{
 if (station_data == 0 || station_data == 2 )
return 1;
else
return 0;
}

//---Timer interrupt----
ISR(TIMER0_COMPA_vect) // Timer interrupt to update steering
{
	//Size on line_data? specify! XXX
	
	uint16_t line_data = request_line_data(); //Collect line data from sensor unit
	int8_t curr_error = (uint8_t)(line_data) - 127;
	uint8_t station_data = (uint8_t)(line_data >> 8);
	
	//pd_update(curr_error);
	//steering_algorithm();

if (!is_station(station_data))	// Continue following line
	{
		pd_update(curr_error);
		steering_algorithm();
		return;
	}
	
	stop_wheels();
	if (station_data == 0)
	{
		display_text("station", "left");
	}
	else if (station_data == 2)
	{
		display_text("statIon", "right");
	}
	else
	{
		display_text("unknown", "error");
	}
	TIMSK0 = (TIMSK0 & (0 << OCIE0A)); // Disable timer-interrupt since waiting for Arm!  reg TIMSK0 bit OCIE0A = 0
}


	/*
	if(!is_right_station_mem()) // Continue following line
	{
		send_decision_to_pc(WRONG_STATION_MEM) ;
		follow_line(line_data);
		reti();
	}
	
	stop_wheels(); //stop robot. Perhaps do this before if!(is_right_station_mem())
	send_decision_to_pc(AT_STATION);
	
	uint8_t time_for_station;
	uint32_t rfid_station = request_rfid_data();
	if (rfid_station == carrying_rfid) // Put down holding object here?
	{
		put_down_object();	
		time_for_station = save_time_and_rfid(rfid_station);
		send_decision_to_pc(OBJECT_IS_PUT_DOWN);
		send_rfid_to_pc(rfid_station, time_for_station);
		follow_line(line_data);
		reti();
	}
	else if (carrying_rfid != 0) // Carrying object but no match for RFID
	{
		save_time_and_rfid(rfid_station);
		send_decision_to_pc(WRONG_STATION_RFID);
		send_rfid_to_pc(rfid_station, time_for_station);
		follow_line(line_data);
		reti();
	}
	
	pick_up_object();
	time_for_station = save_time_and_rfid(carrying_rfid);
	send_rfid_to_pc(carrying_rfid, time_for_station);
	send_decision_to_pc(LOOK_FOR_OBJECT);
	carrying_rfid = rfid_station; // Will be set to 0 later if no object found in ISR(interrupt_arm)
	
	TIMSK0 &= (0b11111101); // Disable timer-interrupt since waiting for Arm!  reg TIMSK0 bit OCIE0A = 0
	reti();
}




ISR(interrupt_arm) // XX Name for interrupt vector??
{
	TIMSK0 |= (1 << OCIE0A); // Enable timer-interrupt again
// object found according to arm? save as carrying_rfid (0 if not) XX

	if (benefit_turn_around())
	{
		turn_around();
	}
}
*/

int main(void)
{
	bus_init(1);
	_delay_ms(100);
	engine_init();
	regulator_init();
	lcd_interface_init();

	//enable timer interrupts for ocie0a
	//sei();
	TIMSK0 |= (1 << OCIE0A);
	TIFR0 |= (1 << OCF0A);
	
	// interrupt frequency 30hz
	OCR0A = 0xff; 
	
	// set to mode 2 (CTC) => clear TCNT0 on compare match
	TCCR0A |= (1 << WGM01 | 0 << WGM00); 
	TCCR0B |= (0 << WGM02);
	
	//prescale
	TCCR0B |= (1 << CS02 | 0 << CS01 | 1 << CS00);
	
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}