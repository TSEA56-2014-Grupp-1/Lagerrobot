/*
 * Chassi.c
 *
 * Created: 3/28/2014 3:41:09 PM
 *  Author: johli887
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

// Decisions
const uint8_t AT_STATION = 1;
const uint8_t WRONG_STATION_MEM = 2;
const uint8_t WRONG_STATION_RFID = 3;
const uint8_t OBJECT_IS_PUT_DOWN = 4;
const uint8_t LOOK_FOR_OBJECT = 5;

// Global variables
uint8_t carrying_rfid = 0;
uint64_t Time_since_start = 0;
// Functions 
uint8_t is_station(uint16_t line_data) {}
void follow_line(uint8_t line_data) {}
void stop_wheels() {}
void send_decision_to_pc(uint8_t decision){} // send data to pc
void send_rfid_to_pc(uint8_t rfid, uint8_t time_for_station){} // send data to pc
uint8_t is_right_station_mem(){} 
uint8_t save_time_and_rfid(uint8_t carrying_rfid) {}
uint16_t request_line_data(){} // send command to Sensor
uint32_t request_rfid_data(){} // send command to Sensor
void put_down_object() {} // send command to Arm
void pick_up_object() {} // send command to Arm
uint8_t benefit_turn_around(){}
void turn_around() {}
	
ISR(TIMER0_COMPA_vect) // Timer interrupt to update steering
{
	//Size on line_data? specify! XXX
	uint16_t line_data = request_line_data(); //Collect line data from sensor unit
	if (!is_station(line_data))	// Continue following line
	{
		follow_line((uint8_t)line_data);
		reti();
	}
	
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

int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code 
    }
}