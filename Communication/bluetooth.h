/*
 * bluetooth.h
 *
 * Created: 2014-04-01 15:35:27
 *  Author: Karl
 */ 


#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <avr/io.h>

void bt_init();

void bt_wait_tx_done();
void bt_wait_rx_done();
void bt_wait_ready_to_tx(); 

void bt_send_byte(uint8_t data);
void bt_process_packet(uint8_t id, uint8_t parameters[]);



#endif /* BLUETOOTH_H_ */