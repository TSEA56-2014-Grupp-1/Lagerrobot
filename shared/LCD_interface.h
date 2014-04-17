#ifndef LCD_INTERFACE_H_
#define LCD_INTERFACE_H_

#include <avr/io.h>
#include <stdarg.h>
#include <stdio.h>

/**
 * @brief The symbols to be displayed.
 */
char lcdi_display_symbols[2][17];

/**
 * @brief Registers the symbol request with the bus.
 * @details Masks the bus plumbing from the lcd user.
 */
void lcdi_init();

/**
 * @brief Clears the display page of the current unit.
 * @details Sets all display positions to space (' ').
 */
void lcdi_display_clear();

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

/**
 * @brief Responds to a request for display symbols.
 * @details A standard bus callback for a request from the communication module for symbols to 
 * print on the display. The request is made with a symbol pair index as metadata, and returns 
 * two symbols. Example, the index 0 will return the first and second symbol, index 2 the fifth and sixth.
 * 
 * @param id Standard callback parameter, the id of the request.
 * @param data Standard callback parameter, here it is the index of the symbol pair requested.
 * 
 * @return [description]
 */
uint16_t lcdi_symbol_request(uint8_t id, uint16_t data);

#endif