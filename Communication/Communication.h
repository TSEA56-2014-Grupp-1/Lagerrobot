/*
 * Communication.h
 *
 * Created: 2014-04-04 11:01:55
 *  Author: Karl
 */


#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <avr/io.h>

#ifndef F_CPU
	#define F_CPU 18432000UL
#endif


#define COMM 0
#define SENS 1
#define ARM 2
#define CHAS 3

/**
 * The first line of text for all pages.
 */
char message_map_line1[4][17];

/**
 * The second line of text for all pages.
 */
char message_map_line2[4][17];

/**
 * Identifier of the unit that is currently being displayed.
 */
uint8_t lcd_next_sender;

/**
 * Keeps track of when it is time to switch pages.
 */
uint8_t lcd_rotation_counter;

uint8_t lcd_rotation_flag;
/**
 * @brief Forces the display to display the page of a certain module.
 * @details Resets the rotation counter and outputs the page of a certain module to the display.
 *
 * @param module The identifier of the module to be displayed.
 */
void lcd_force_display_update(uint8_t module);


void lcd_process_symbol(uint8_t module, uint8_t line_number, uint16_t metadata);

/**
 * @brief Clears the display page of a unit.
 * @details Clears the stored display page of a unit, but does not update the display.
 *
 * @param unit The identifier of the module whose page is to be cleared.
 */
void clear_message(uint8_t unit, uint8_t line_number);
#endif /* COMMUNICATION_H_ */
