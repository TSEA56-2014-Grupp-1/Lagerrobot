/*
 * Armenhet.c
 *
 * Created: 3/25/2014 4:23:12 PM
 *  Author: eriny778
 */ 

#include "armenhet.h"
#include "servocomm.c"


//------Functions-------

/*
RxPacket() read data from buffer.
RxPacket() need a Parameter; Total length of Return Packet.
RxPacket() return Length of Return Packet.
*/

ISR(USART0_RX_vect)
{
	recieve_interrupt_buffer[receive_buffer_write_pointer++] = UDR0;
}


void servo_init()
{
	//transfer_packet();
}

void move_joint (int joint, int goal_pos)
	{
		
	}
	

void move_servo (uint8_t id, int goal_angle)
{
	
	uint16_t bit_angle;
	uint8_t h_goal_pos, l_goal_pos;

	bit_angle = (uint16_t)(goal_angle*2.93);
	l_goal_pos = (uint8_t)(bit_angle);
	h_goal_pos = (uint8_t)(bit_angle >> 8);
	
	parameter[0] = GOAL_POSITION_L;
	parameter[1] = l_goal_pos;
	parameter[2] = h_goal_pos;
	
	transfer_packet(id, INST_WRITE, 3);	recieve_packet(DEFAULT_RETURN_PACKET_SIZE);	}



int main(void)
{

	UBRR0H = 0x00;
	UBRR0L = 0x00;
	

	
	sei();
	
	DDRD = (0x1<<DDD2);
	
// 	move_servo (0x07, 180);
// 	_delay_ms(240);
// 	move_servo(0x01, 180);

	
	while(1)
    {
	
    }
	
}