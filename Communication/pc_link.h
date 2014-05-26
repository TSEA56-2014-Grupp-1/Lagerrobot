#ifndef PC_LINK_H_
#define PC_LINK_H_
/**
 *	@file pc_link.h
 *	@author Andreas Runfalk, Karl Linderhed and Patrik Nyberg
 *
 *	For sending and receiving packets to and from PC
 */

#ifndef F_CPU
	#define F_CPU 18432000UL
#endif

#include <avr/io.h>

uint8_t process_packet();
void send_packet(uint8_t packet_id, uint8_t num_parameters, ...);

#endif /* PC_LINK_H_ */
