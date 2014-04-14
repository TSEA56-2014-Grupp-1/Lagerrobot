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
uint16_t read_RFID(uint8_t id, uint16_t metadata);
uint8_t RFID_read_usart();
uint8_t compare_RFID_arrays(const uint8_t current_compare_RFID[11]);
uint8_t identify_station_RFID();
void RFID_disable_reading(uint8_t id, uint16_t metadata);
void RFID_enable_reading(uint8_t id, uint16_t metadata);

#endif /* RFID-SCANNER_H_ */