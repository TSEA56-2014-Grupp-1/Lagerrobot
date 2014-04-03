/*
 * RFID_scanner.h
 *
 * Created: 2014-04-03 13:40:09
 *  Author: Erik
 */ 


#ifndef RFID_SCANNER_H_
#define RFID_SCANNER_H_


/*
#define RFID_B80 2600DB916A
#define RFID_B81 2600DB27C4
#define RFID_B82 2600D8AD0E
#define RFID_B83 2600D8AB8B
#define RFID_B84 2600D3D82F
#define RFID_B85 2600D3DBB8
*/

const uint8_t  RFID_B80[] = {
	0x0A, 0x32, 0x36, 0x30, 0x30, 0x44, 0x42, 0x39, 0x31, 0x36, 0x41, 0x0D
};  
const uint8_t RFID_B81[] = {
	0x0A, 0x32, 0x36, 0x30, 0x30, 0x44, 0x42, 0x32, 0x37, 0x43, 0x34, 0x0D
};
const	uint8_t RFID_B82[] = {
	0x0A, 0x32, 0x36, 0x30, 0x30, 0x44, 0x38, 0x41, 0x44, 0x30, 0x45, 0x0D
};
const	uint8_t RFID_B83[] = {
	0x0A, 0x32, 0x36, 0x30, 0x30, 0x44, 0x38, 0x41, 0x42, 0x38, 0x42, 0x0D
};
const uint8_t RFID_B84[] = {
	0x0A, 0x32, 0x36, 0x30, 0x30, 0x44, 0x33, 0x44, 0x38, 0x32 ,0x46, 0x0D
};
const uint8_t RFID_B85[] = {
	0x0A, 0x32, 0x36, 0x30, 0x30, 0x44 ,0x33, 0x44, 0x42, 0x42, 0x38, 0x0D
};
/*
vector RFID_vector[5] ={
	 RFID_B80, RFID_B81, *RFID_B82, *RFID_B83, *RFID_B84, *RFID_B85
};
*/
void init_RFID_scanner();
void read_RFID();



#endif /* RFID-SCANNER_H_ */