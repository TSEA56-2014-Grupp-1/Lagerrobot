/*
 * Communication.h
 *
 * Created: 2014-04-04 11:01:55
 *  Author: Karl
 */ 


#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <avr/io.h>

#define F_CPU 18432000UL


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
uint8_t lcd_current_sender;

/**
 * Keeps track of when it is time to switch pages.
 */
uint8_t lcd_rotation_counter;

/**
 * @brief Clears the display page of a unit.
 * @details Clears the stored display page of a unit, but does not update the display.
 * 
 * @param unit The identifier of the module whose page is to be cleared.
 */
void clear_message(uint8_t unit);

#endif /* COMMUNICATION_H_ */