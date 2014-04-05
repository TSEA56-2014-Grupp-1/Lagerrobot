/*
 * bluetooth.c
 *
 * Created: 2014-04-01 15:35:48
 *  Author: Karl
 */ 

#include "bluetooth.h"
#include "Communication.h"
#include "../shared/packets.h"
#include "../shared/usart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void bt_process_packet(uint8_t packet_id, uint8_t parameters[]) {
	switch (packet_id) {
		case PKT_STOP:
			
		break;
		case PKT_ARM_COMMAND:
			switch (parameters[0]) {
				case CMD_ARM_MOVE:
					
				break;
				case CMD_ARM_GRIP:
				
				break;
				case CMD_ARM_RELEASE:
				
				break;
				case CMD_ARM_PREDEFINED_POS:
				
				break;
			}
		break;
		case PKT_CHASSIS_COMMAND:
			switch (parameters[0]) {
				case CMD_CHASSIS_SPEED:
				
				break;
				case CMD_CHASSIS_STEER:
				
				break;
				case CMD_CHASSIS_START:
				
				break;
			}
		break;
		case PKT_CALIBRATION_COMMAND:
		
		break;
		case PKT_PACKET_REQUEST:
		
		break;
		case PKT_SPOOFED_REQUEST:
		
		break;
		case PKT_SPOOFED_TRANSMIT:
		
		break;
	}
}


