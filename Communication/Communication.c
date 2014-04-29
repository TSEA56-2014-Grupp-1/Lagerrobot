/*
 * Communication.c
 *
 * Created: 2014-03-26 09:49:31
 *  Author: Karl
 */
#define ROTATE_INTERVAL 100

#include "Communication.h"
#include "LCD.h"

#include "pc_link.h"
#include "../shared/usart.h"
#include "../shared/bus.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include "../shared/packets.h"

ISR(TIMER1_OVF_vect) {
	if(lcd_rotation_counter == ROTATE_INTERVAL) {
		lcd_rotation_counter = 0;

		lcd_display(lcd_current_sender,
		message_map_line1[lcd_current_sender],
		message_map_line2[lcd_current_sender]);
		if (lcd_current_sender == 3)
		lcd_current_sender = 0;
		else
		++lcd_current_sender;
	}
	else
	++lcd_rotation_counter;
}

/**
* @brief Callback function that indicates a unit is ready to send symbols to the display.
* @details This is a standard callback for a bus_transmit. It will perform a request for each symbol pair in turn from the unit before it forces a display update.
*
* @param id Standard callback parameter, the id of the transmission.
* @param data Standard callback parameter, the three highest bits denote which line is to be displayed, the lower byte is
* the address of the unit that wants to use the display.
*/
void symbols_are_ready(uint8_t id, uint16_t data) {
	uint16_t symbol_pair;
	uint8_t line_number;
	uint8_t module;
	switch((uint8_t) data) {
		case BUS_ADDRESS_ARM:
		module = ARM;
		break;
		case BUS_ADDRESS_CHASSIS:
		module = CHAS;
		break;
		case BUS_ADDRESS_SENSOR:
		module = SENS;
		break;
		default:
		module = 0;
	}

	line_number = (uint8_t) (data >> 8);

	if (line_number < 2) {
		clear_message(module, line_number);

		for (int i = 0; i < 8; ++i) {
			bus_request((uint8_t) data, 1, i, &symbol_pair);

			if (line_number == 0) {
				message_map_line1[module][2*i] = symbol_pair >> 8;
				message_map_line1[module][2*i+1] = symbol_pair;

			}
			else if (line_number == 1) {
				message_map_line2[module][2*i] = symbol_pair >> 8;
				message_map_line2[module][2*i+1] = symbol_pair;
			}

		}
		lcd_force_display_update(module);
	}
}

/**
* @brief Clears the display page of a unit.
* @details Clears the stored display page of a unit, but does not update the display.
*
* @param unit The identifier of the module whose page is to be cleared.
*/
void clear_message(uint8_t unit, uint8_t line_number) {
	if (line_number == 0) {
		for (int i = 0; i<16; ++i){
			message_map_line1[unit][i] = 0x20;
		}
		message_map_line1[unit][16] = '\0';
	}
	else if (line_number == 1) {
		for (int i = 0; i<16; ++i){
			message_map_line2[unit][i] = 0x20;
		}
		message_map_line2[unit][16] = '\0';
	}


}


/**
* @brief Initializes the communication unit.
* @details Sets up the ports for the display communication, the timers for
* page rotation and clears the lcd variables and messages.
*/
void init(){
	DDRA = 0xff;
	DDRB = 0xff;
	PORTB = 0b00000000;
	PORTA = 0x0;

	TIMSK1 = (1 << TOIE1);
	TCCR1A = 0x00; // normal mode
	TCCR1B = 0b00000010; // normal mode, max prescaler;

	lcd_current_sender = 0;
	lcd_rotation_counter = 0;
	clear_message(COMM, 0);
	clear_message(COMM, 1);
	clear_message(SENS, 0);
	clear_message(SENS, 1);
	clear_message(CHAS, 0);
	clear_message(CHAS, 1);
	clear_message(ARM, 0);
	clear_message(ARM, 1);
}

void forward_calibration_data(uint8_t id, uint16_t metadata)	{
	send_packet(PKT_CALIBRATION_DATA,1,(uint8_t)metadata);
}

int main(void)
{
	uint8_t pp_status = 0;
	init();
	lcd_init();

	bus_init(BUS_ADDRESS_COMMUNICATION);

	bus_register_receive(2, symbols_are_ready);

	display(0, "Ouroborobot");
	display(1, "Startup.");
	_delay_ms(300);
	display(0, "Ouroborobot");
	display(1, "Startup..");
	_delay_ms(300);
	display(0, "Ouroborobot");
	display(1, "Startup...");
	_delay_ms(300);

	clear_message(COMM, 0);
	clear_message(COMM, 1);
	for(;;)	{
		while (!usart_has_bytes());

		pp_status = process_packet();
		display(1, "P-Status: %u", pp_status);

		usart_reset_buffer();
	}
}
