/*
 * Communication.h
 *
 * Created: 2014-04-04 11:01:55
 *  Author: Karl
 */ 


#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <avr/io.h>

#define F_CPU 18.432E6


#define COMM 0
#define SENS 1
#define ARM 2
#define CHAS 3

char message_map_line1[4][17];

char message_map_line2[4][17];

uint8_t lcd_current_sender;

#endif /* COMMUNICATION_H_ */