#ifndef PACKETS_H_
#define PACKETS_H_

/*	packets					|	packet_id | packet contents (byte numbers)			*/			
#define PKT_STOP				0	//		none
#define PKT_ARM_COMMAND			1	//		command type(1), data(2-4)
#define PKT_CHASSIS_COMMAND		2	//		command type(1), data(2-3)
#define PKT_CALIBRATION_COMMAND	3	//		sensor id(1)
#define PKT_ARM_STATUS			4	//		<not defined yet>
#define PKT_CHASSIS_STATUS		5	//		<not defined yet>
#define PKT_LINE_DATA			6	//		sensor values(1-11), line flags(12), center of mass(13)
#define PKT_RANGE_DATA			7	//		ranger values(1-3), distances(4-6), angles(7-8) 
#define PKT_RFID_DATA			8	//		tag bytes(1-12)
#define	PKT_CHASSIS_DECISION	9	//		<not defined yet>
#define PKT_ARM_DECISION		10	//		<not defined yet>
#define PKT_PACKET_REQUEST		11	//		id of packet pc wants(1)
#define PKT_SPOOFED_REQUEST		12	//		bus request packets(1-4) (address, id, metadata_h, metadata_l)
#define PKT_SPOOFED_RESPONSE	13	//		bus response packets(1-2)
#define PKT_SPOOFED_TRANSMIT	14	//		bus transmit packets(1-4) (address, id, metadata_h, metadata_l)
#define PKT_CALIBRATION_DATA    15  //      calibration value (1)


/*	commands (in packets)	|	cmd_id	  |	command data					*/ 
#define CMD_ARM_MOVE			0	//		direction(1), joint(2)
#define CMD_ARM_GRIP			1	//		none
#define CMD_ARM_RELEASE			2	//		none
#define CMD_ARM_PREDEFINED_POS	3	//		predefine id(1)
#define CMD_ARM_STOP            4   //      joint(1)

#define CMD_CHASSIS_SPEED		0	//		speed(1)
#define CMD_CHASSIS_STEER		1	//		steering power(1)
#define	CMD_CHASSIS_START		2	//		none
#define CMD_CHASSIS_PARAMETERS	3	//		Kp value(1), Kd value(2)
#define CMD_CHASSIS_MOVEMENT    4   //      command


/*	 calibration commands	cmd_id			parameters			*/
#define CAL_LINE				1	//		tape or no tape(1)
#define CAL_RANGE				2	//		distance to object(1)


#endif /* PACKETS_H_*/
