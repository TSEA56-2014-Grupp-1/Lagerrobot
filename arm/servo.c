#include <avr/io.h>
#include "arm.h"
#include "servo.h"

#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

uint8_t recieve_interrupt_buffer[256];
uint8_t receive_buffer_read_pointer;
uint8_t receive_buffer_write_pointer;

void servo_init()
{
	UBRR0H = 0x00;
	UBRR0L = 0x00;
	DDRD = 0x1 << DDD2;
}

servo_command servo_make_command(
	uint8_t servo_id, uint8_t address_id, uint8_t data_length, uint8_t data[])
{
	uint8_t i;
	servo_command *command = malloc(
		sizeof(servo_command) + data_length * sizeof(uint8_t));

	command->id = servo_id;
	command->address = address_id;

	// XXX: Use memcpy?
	for (i = 0; i < data_length; i++) {
		command->data[i] = data[i];
	}

	return *command;
}

servo_response servo_make_response(
	uint8_t id, uint8_t error, uint8_t data_length, uint8_t data[])
{
	uint8_t i;
	servo_response *response = malloc(
		sizeof(servo_response) + sizeof(uint8_t) * data_length);

	response->id = id;
	response->error = error;
	response->data_length = data_length;

	// XXX: Use memcpy?
	for (i = 0; i < data_length; i++) {
		response->data[i] = data[i];
	}

	return *response;
}

ISR(USART0_RX_vect)
{
	recieve_interrupt_buffer[receive_buffer_write_pointer++] = UDR0;
}

void servo_transfer_byte(uint8_t data)
{
	while (SERVO_CHECK_TRANSFER_READY); //wait until data can be loaded.
	UDR0 = data; //data load to TxD buffer
}


void servo_enable_transmit()
{
	UCSR0B = ((0x0<<RXCIE0) | (0x0<<RXEN0) | (0x1<<TXEN0));
	PORTD = (0x1<<PORTD2);

	// Wait for tri-state buffer to switch directions
	_delay_us(10);
}

void servo_enable_receive()
{
	// TODO: Verify this delay
	_delay_ms(1);

	UCSR0B = ((0x1<<RXCIE0) | (0x1<<RXEN0) | (0x0<<TXEN0));
	PORTD = (0x0<<PORTD2);
}

/*uint8_t recieve_packet(uint8_t recieve_packet_length, uint8_t transfer_buffer[128])
{
	uint8_t recieve_buffer[128];

	#define RECEIVE_TIMEOUT_COUNT 30000UL
	unsigned long ulCounter;
	uint8_t count, length, checksum;
	uint8_t timeout;

	timeout = 0;
	for(count = 0; count < recieve_packet_length; count++)
	{
		ulCounter = 0;
		while(receive_buffer_read_pointer == receive_buffer_write_pointer)
		{
			if(ulCounter++ > RECEIVE_TIMEOUT_COUNT)
			{
				timeout = 1;
				break;
			}
		}
		if(timeout) break;
		recieve_buffer[count] =
		recieve_interrupt_buffer[receive_buffer_read_pointer++];
	}
	length = count;
	checksum = 0;

	if(transfer_buffer[2] != BROADCASTING_ID)
	{
		if(timeout && recieve_packet_length != 255)
		{
			//TxDString("\r\n [Error:RxD Timeout]");
			CLEAR_BUFFER;
		}

		if(length > 3) //checking is available.
		{
			if(recieve_buffer[0] != 0xff || recieve_buffer[1] != 0xff )
			{
				//TxDString("\r\n [Error:Wrong Header]");
				CLEAR_BUFFER;
				return 0;
			}
			if(recieve_buffer[2] != transfer_buffer[2] )
			{
				//TxDString("\r\n [Error:TxID != RxID]");
				CLEAR_BUFFER;
				return 0;
			}
			if(recieve_buffer[3] != length-4)
			{
				//TxDString("\r\n [Error:Wrong Length]");
				CLEAR_BUFFER;
				return 0;
			}
			for(count = 2; count < length; count++) checksum +=
			recieve_buffer[count];
			if(checksum != 0xff)
			{
				//TxDString("\r\n [Error:Wrong CheckSum]");
				CLEAR_BUFFER;
				return 0;
			}
		}
	}
	return (length, recieve_buffer);
}*/


/*uint8_t transfer_packet(uint8_t id, uint8_t instruction, uint8_t parameter_length)
{
	uint8_t count,checksum,packet_length;
	uint8_t temp_transfer_buffer[128];

	temp_transfer_buffer[0] = 0xff;
	temp_transfer_buffer[1] = 0xff;
	temp_transfer_buffer[2] = id;
	temp_transfer_buffer[3] = parameter_length+2;//Length(Paramter,Instruction,Checksum)
	temp_transfer_buffer[4] = instruction;

	for(count = 0; count < parameter_length; count++) // inserts parameters into transferbuffer
	{
		temp_transfer_buffer[count+5] = parameter[count];
	}

	checksum = 0;
	packet_length = parameter_length+4+2; // total lenght of send packet

	for(count = 2; count < packet_length-1; count++) //sums packet except 0xff & checksum
	{
		checksum += temp_transfer_buffer[count];
	}

	temp_transfer_buffer[count] = ~checksum; // writing checksum with bit inversion

	enable_transmit();

	for(count = 0; count < packet_length; count++)
	{
		//sbi(UCSR0A,6);//SET_TXD0_FINISH;
		transfer_byte(temp_transfer_buffer[count]);
	}

	while(!CHECK_TRANSFER_FINISH); //Wait until TXD Shift register empty

	_delay_ms(1);
	enable_recieve();

	recieve_packet(DEFAULT_RETURN_PACKET_SIZE, temp_transfer_buffer);

	return(packet_length, temp_transfer_buffer);

}*/

servo_response servo_receive_packet() {
	//uint8_t recieve_buffer[128];

	uint8_t timeout;
	uint16_t timeout_counter = 0;

	uint8_t temp_byte;

	uint8_t address_id;
	uint8_t packet_length = 0;
	uint8_t error;
	uint8_t* parameters;
	uint8_t checksum = 0;

	uint8_t index = 0;
	while (1) {
		while (receive_buffer_read_pointer == receive_buffer_write_pointer) {
			if (timeout_counter++ >= SERVO_RECEIVE_TIMEOUT_COUNT) {
				timeout = 1;
				break;
			}
		}

		// Check if any data was received before timeout
		if (timeout == 1) {
			break;
		}

		temp_byte = recieve_interrupt_buffer[receive_buffer_read_pointer++];

		switch (index) {
			case 0:
			case 1:
				// Start bytes, ignore
				break;
			case 2:
				// Servo ID
				address_id = temp_byte;
				checksum += temp_byte;
			case 3:
				// Length
				packet_length = temp_byte;
				checksum += temp_byte;

				// Allocate space for parameter array if we expect parameters
				if (packet_length > 2) {
					parameters = malloc(sizeof(uint8_t) * packet_length - 2);
				}
				break;
			case 4:
				// Error
				error = temp_byte;
				checksum += temp_byte;
			default:
				// If no packet length was set something went wrong
				if (packet_length == 0) {
					SERVO_CLEAR_BUFFER;
					return;
				}

				if (index - 5 >= packet_length) {
					// TODO: Verify checksum

					return servo_make_response(
						address_id, error, packet_length - 2, parameters);
				} else {
					parameters[index - 5] = temp_byte;
				}
		}

		index++;
	}

	SERVO_CLEAR_BUFFER;
}

servo_response servo_transfer_command(uint8_t instruction, servo_command cmd) {
	uint8_t count;
	uint8_t checksum;

	uint8_t transfer_length = cmd.data_length + 7;
	uint8_t temp_transfer_buffer[transfer_length];

	temp_transfer_buffer[0] = 0xff;
	temp_transfer_buffer[1] = 0xff;
	temp_transfer_buffer[2] = cmd.id; // Servo ID
	temp_transfer_buffer[3] = cmd.data_length + 3; // data_length + address + instruction + checksum
	temp_transfer_buffer[4] = instruction; // Read, write, etc
	temp_transfer_buffer[5] = cmd.address;

	// Add data
	for (count = 0; count < cmd.data_length; count++) {
		temp_transfer_buffer[count + 6] = cmd.data[count];
	}

	// Calculate checksum
	for (count = 2; count < transfer_length - 1; count++) {
		checksum += temp_transfer_buffer[count];
	}
	temp_transfer_buffer[transfer_length - 1] = ~checksum;

	// Disable interrupts while sending bytes
	cli();
	servo_enable_transmit();

	for (count = 0; count < transfer_length; count++) {
		servo_transfer_byte(temp_transfer_buffer[count]);
	}

	// Wait for transfer shift register to finish sending bytes
	while(!SERVO_CHECK_TRANSFER_FINISH);

	// Re-enable interrupts as soon as bytes should be received
	servo_enable_receive();
	sei();

	if (cmd.address) {

	}

	return servo_receive_packet();
}

void servo_buffer_move(uint8_t id, uint16_t goal_angle)
{
	uint16_t bit_angle;
	uint8_t h_goal_pos, l_goal_pos;

	if (id > 0 && id <= 8)
	{
		bit_angle = (uint16_t)(goal_angle*2.93);
		l_goal_pos = (uint8_t)(bit_angle);
		h_goal_pos = (uint8_t)(bit_angle >> 8);

		servo_transfer_command(
			SERVO_INST_WRITE,
			servo_make_command(
				id,
				SERVO_GOAL_POSITION_L,
				2,
				(uint8_t[]){l_goal_pos, h_goal_pos}));

	}
	else
	{
		//ERROR: invalid servo-ID
	}

}

void servo_buffer_commit()
{
	servo_transfer_command(
		SERVO_INST_ACTION,
		servo_make_command(SERVO_BROADCASTING_ID, 0, 0, (uint8_t[]){}));
}
