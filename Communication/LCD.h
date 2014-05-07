/*
 * LCD.h
 *
 * Created: 2014-03-25 17:29:45
 *  Author: Karl Linderhed
 */ 


#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>

/**
 * @brief Displays a formatted string of text and variables on the display.
 * @details This is a special function for the Communication unit, since it can't
 * use the LCD_interface to send text across the bus. It encapsulates a standard printf
 * function that can take any number of variables as parameters.
 * 
 * @param line_number The line on which text is to be printed. 0 is the first line, 1 is the second.
 * @param str The format string to display.
 * @param ... The variables to display.
 */
void display(uint8_t line_number, char* str, ...);

/**
 * @brief Sends a single symbol to the display.
 * @details This will send a symbol to the current DDRAM address on the display controller
 * and automatically increment the display cursor.
 * 
 * @param symbol The symbol to be printed.
 */
void lcd_send_symbol(char);

/**
 * @brief Sends a command to the display controller.
 * 
 * @param cmd The command code to be sent.
 */
void lcd_send_command(char);

/**
 * @brief Checks if the display is busy.
 * @details Queries the display controller if it is able to receive instructions or data.
 * @return The value of the busy flag. 1 (true) means the display is busy.
 */
int lcd_is_busy();
 
/**
 * @brief Prints two lines of text on the display.
 * 
 * @param text1 The first line of text.
 * @param text2 The second line of text.
 */
void lcd_print(char text1[], char text2[]);

/**
 * @brief Clears the display.
 */
void lcd_clear();

/**
 * @brief Initializes the lcd display controller.
 */
void lcd_init();


void lcd_display(uint8_t sender, char text1[], char text2[]);

#endif /* LCD_H_ */