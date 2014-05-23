#ifndef LCD_INTERFACE_H_
#define LCD_INTERFACE_H_
/**
 *	@file LCD_interface.h
 *	@author Karl Linderhed
 *
 *	Functions for working with LCD display on units connected to the communication
 *	unit.
 */

#include <avr/io.h>
#include <stdarg.h>
#include <stdio.h>

/**
 * @brief Displays a formatted string of text and variables on the display.
 * @details Uses variable arguments to encapsulate a standard printf, which takes
 * a formatted string and any number of variables.
 *
 * @param line_number The line on which text is to be printed. 0 is the first line, 1 is the second.
 * @param str The format string to display.
 * @param ... The variables to display.
 */
void display(uint8_t line_number, const char* str, ...);

#endif
