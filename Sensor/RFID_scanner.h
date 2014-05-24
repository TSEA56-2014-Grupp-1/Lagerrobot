#ifndef RFID_SCANNER_H_
#define RFID_SCANNER_H_
/**
 *	@file RFID_scanner.h
 *	@author Erik Nybom
 *
 *	Functions for reading RFID scanner data
 */

void RFID_scanner_init(void);
uint8_t RFID_read_usart(void);
uint8_t compare_RFID_arrays(const uint8_t current_compare_RFID[11]);
uint8_t identify_station_RFID(void);
void clear_station_RFID(void);
void send_rfid(uint8_t station_tag);

#endif /* RFID-SCANNER_H_ */
