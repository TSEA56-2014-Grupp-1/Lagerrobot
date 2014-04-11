/*
 * RFID_scanner.h
 *
 * Created: 2014-04-03 13:40:09
 *  Author: Erik
 */ 


#ifndef RFID_SCANNER_H_
#define RFID_SCANNER_H_

//----Functions---
void RFID_scanner_init();
void read_RFID();
void RFID_read_usart();
uint8_t compare_RFID_arrays(uint8_t station_RFID[11], const uint8_t current_compare_RFID[11]);
uint8_t identify_station_RFID();
uint16_t return_rfid_tag(uint8_t id, uint16_t metadata);

#endif /* RFID-SCANNER_H_ */