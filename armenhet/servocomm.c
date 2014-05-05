
void transfer_byte(uint8_t data)
{
	while (CHECK_TRANSFER_READY); //wait until data can be loaded.
	UDR0 = data; //data load to TxD buffer
}


void enable_transmit()
{
	UCSR0B = ((0x0<<RXCIE0) | (0x0<<RXEN0) | (0x1<<TXEN0));
	PORTD = (0x1<<PORTD2);
}

void enable_recieve()
{
	UCSR0B = ((0x1<<RXCIE0) | (0x1<<RXEN0) | (0x0<<TXEN0));
	PORTD = (0x0<<PORTD2);
}

uint8_t recieve_packet(uint8_t recieve_packet_length)
{
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
	return length;
}


uint8_t transfer_packet(uint8_t id, uint8_t instruction, uint8_t parameter_length)
{
	uint8_t count,checksum,packet_length;
	
	transfer_buffer[0] = 0xff;
	transfer_buffer[1] = 0xff;
	transfer_buffer[2] = id;
	transfer_buffer[3] = parameter_length+2;//Length(Paramter,Instruction,Checksum)
	transfer_buffer[4] = instruction;
	
	for(count = 0; count < parameter_length; count++) // inserts parameters into transferbuffer
	{
		transfer_buffer[count+5] = parameter[count];
	}
	
	checksum = 0;
	packet_length = parameter_length+4+2; // total lenght of send packet
	
	for(count = 2; count < packet_length-1; count++) //sums packet except 0xff & checksum
	{
		checksum += transfer_buffer[count];
	}
	
	transfer_buffer[count] = ~checksum; // writing checksum with bit inversion
	
	enable_transmit();
	_delay_us(10);
	
	for(count = 0; count < packet_length; count++)
	{
		//sbi(UCSR0A,6);//SET_TXD0_FINISH;
		transfer_byte(transfer_buffer[count]);
	}
	
	while(!CHECK_TRANSFER_FINISH); //Wait until TXD Shift register empty
	
	_delay_ms(1);
	enable_recieve();
	
	return(packet_length);
	
}