/*
 * bluetooth.h
 *
 * Created: 2014-04-01 15:35:27
 *  Author: Karl
 */ 


#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <avr/io.h>

void bt_process_packet(uint8_t id, uint8_t parameters[]);



#endif /* BLUETOOTH_H_ */