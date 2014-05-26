#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_
/**
 *	@file Communication.h
 *	@author Karl Linderhed
 *
 *	Processes commands to and from PC as well as controlling LCD display
 */

#include <avr/io.h>

#ifndef F_CPU
	#define F_CPU 18432000UL
#endif

/**
 *	Index in own arrays for communication LCD data
 */
#define COMM 0

/**
 *	Index in own arrays for sensor LCD data
 */
#define SENS 1

/**
 *	Index in own arrays for arm LCD data
 */
#define ARM 2

/**
 *	Index in own arrays for chassi LCD data
 */
#define CHAS 3

/**
 *	Number of timer overflows before active LCD page switches
 */
#define ROTATE_INTERVAL 100

/**
 * The first line of text for all pages.
 */
char message_map_line1[4][17];

/**
 * The second line of text for all pages.
 */
char message_map_line2[4][17];

/**
 *	Remember if a stop signal was sent and prevent further ones until computer reconnects
 */
uint8_t stop_sent;

/**
 *	Remember if the computer has connected at all
 */
uint8_t has_connection;

/**
 * Identifier of the unit that is currently being displayed.
 */
uint8_t lcd_next_sender;

/**
 * Keeps track of current LCD page
 */
uint8_t lcd_rotation_counter;

/**
 *	Read by main program loop to determine if display should be updated
 */
uint8_t lcd_rotation_flag;

void lcd_force_display_update(uint8_t module);

void lcd_process_symbol(uint8_t module, uint8_t line_number, uint16_t metadata);

void clear_message(uint8_t unit, uint8_t line_number);

#endif /* COMMUNICATION_H_ */
