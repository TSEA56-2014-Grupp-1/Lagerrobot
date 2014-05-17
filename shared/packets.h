#ifndef PACKETS_H_
#define PACKETS_H_

/*	packets					|	packet_id | packet contents (byte numbers)			*/			
#define PKT_STOP				0	//		none
#define PKT_ARM_COMMAND			1	//		command type(1), data(2-10)
#define PKT_CHASSIS_COMMAND		2	//		command type(1), data(2-3)
#define PKT_CALIBRATION_COMMAND	3	//		sensor id(1)
#define PKT_ARM_STATUS			4	//		<not defined yet>
#define PKT_CHASSIS_STATUS		5	//		<not defined yet>
#define PKT_LINE_DATA			6	//		sensor values(1-11), line flags(12), center of mass(13), steering wheel(14-15)
#define PKT_RANGE_DATA			7	//		side of sensor(1), distance (2-3) 
#define PKT_RFID_DATA			8	//		tag bytes(1)
#define	PKT_CHASSIS_DECISION	9	//		type of decision (1)
#define PKT_ARM_DECISION		10	//		<not defined yet>
#define PKT_PACKET_REQUEST		11	//		id of packet pc wants(1)
#define PKT_SPOOFED_REQUEST		12	//		bus request packets(1-4) (address, id, metadata_h, metadata_l)
#define PKT_SPOOFED_RESPONSE	13	//		bus response packets(1-2)
#define PKT_SPOOFED_TRANSMIT	14	//		bus transmit packets(1-4) (address, id, metadata_h, metadata_l)
#define PKT_CALIBRATION_DATA    15  //      calibration value (1)
#define PKT_HEARTBEAT           16  //      none

/*	commands (in packets)	|	cmd_id	  |	command data					*/ 
#define CMD_ARM_MOVE			0	//		coordinate(1) [x = 0, y = 1, angle = 2], direction(2) [1=up, 0=down], start/stop(3) [start = 1, stop = 0]
#define CMD_ARM_GRIP			1	//		none
#define CMD_ARM_RELEASE			2	//		none
#define CMD_ARM_PREDEFINED_POS	3	//		predefine id(1)
#define CMD_ARM_STOP            4   //      joint(1)
#define CMD_ARM_MOVE_POS        5   //      x_h(1), x_l(2), y_h(3), y_l(4), angle(5)

#define CMD_CHASSIS_SPEED		0	//		speed(1)
#define CMD_CHASSIS_STEER		1	//		steering power(1)
#define	CMD_CHASSIS_START		2	//		none
#define CMD_CHASSIS_PARAMETERS	3	//		Kp value(1), Kd value(2)
#define CMD_CHASSIS_MOVEMENT    4   //      command


/*	 calibration commands	cmd_id			parameters			*/
#define CAL_LINE				1	//		tape or no tape(1)
#define CAL_RANGE				2	//		distance to object(1)

/*	Types of decision			dec_id */
#define DEC_PICKUP_RIGHT		0   //Station to the right
#define DEC_PICKUP_LEFT			1	//Station to the left
#define DEC_PUT_DOWN_RIGHT		2	//Putting down object to the right
#define DEC_PUT_DOWN_LEFT		3	//Putting down object to the left
#define DEC_NO_MATCH			4	//ID did not match object
#define DEC_STATION_HANDELED	5	//Station is already handeld
#define DEC_NO_ID_FOUND			6	//No ID was found
#define DEC_ARM_PICKED_UP		7	//Arm has picked up an object
#define DEC_ARM_PUT_DOWN		8	//Arm has put down the object
#define DEC_OBJECT_NOT_FOUND	9	//Object was not found by arm
#define DEC_UNKOWN_ERROR		10	//An unkown error has occured

#endif /* PACKETS_H_*/
