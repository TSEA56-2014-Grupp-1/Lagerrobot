#include <avr/io.h>
#include <avr/interrupt.h>

int16_t bus_data;
int16_t test_var_for_bus;

void bus_init(uint8_t address) {
	// Modify bus frequency
	// 18200000/(16+2*TWBR*(4^TWSR prescaler)) = 8.8 kHz (testing shows 15.5kHz ...)
	// http://www.avrbeginners.net/architecture/twi/twi.html
	TWBR = 0xff;
	//TWBR = 0x0D; // Works good (~192kHz)

	// Enable internal pull-up for SDA och SCL
	PORTC |= 0x03;

	// Set address on bus
	TWAR = (address << 1) | 1;

	// Enable the bus
	TWSR &= 0xfc;
	TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE);
}

/**
 *	Try to aquire control over the bus
 */
uint8_t bus_start(void) {
	TWCR |= (1 << TWINT) |(1 << TWSTA);

	// Wait for TWINT to go high
	while (!(TWCR & (1 << TWINT)));

	// Check if failure to seize bus
	if ((TWSR & 0xf8) == 0xf8) {
		return 1;
	}

	return 0;
}

/**
 *	Let go of the control over the buss
 */
void bus_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

/**
 *	Write a byte of data on the bus
 */
uint8_t bus_write(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);

	// Wait for TWINT to go high (package sent)
	while (!(TWCR & (1 << TWINT)));

	return TWSR & 0xf8;
}

/**
 *	Read one byte from the bus and send ACK to indicate we expect more packets
 */
uint8_t bus_read_ack(uint8_t* data) {
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

	// Wait for TWINT to go high
	while (!(TWCR & (1 << TWINT)));

	*data = TWDR;

	return TWSR & 0xf8;
}

/**
 *	Read one byte from the bus and send NACK to indicate we expect no more packets
 */
uint8_t bus_read_nack(uint8_t* data) {
	TWCR = (1 << TWINT) | (1 << TWEN);

	// Wait for TWINT to go high
	while (!(TWCR & (1 << TWINT)));

	*data = TWDR;

	return TWSR & 0xf8;
}

/**
 *	Calculate data byte for addressing the bus
 *
 *	@param	7-bit address for target, the 7th bit is ignored
 *	@param	1-bit to set send or receive, only the 0th bit is used. 0 means send, 1 means receive
 */
uint8_t bus_calculate_address_packet(uint8_t address, uint8_t direction) {
	if (direction) {
		return (address << 1) | 1;
	} else {
		return (address << 1) & 0xfe;
	}
}

/**
 *	Send two bytes of data over the bus
 *
 *	This function disables interrupts at the start and reenables then when done
 */
uint8_t bus_send(uint8_t address, uint8_t byte1, uint8_t byte2) {
	// XXX: Are interrupts always enabled when this function is called
	cli();

	if (bus_start() != 0) {
		return 1;
	}

	if (bus_write(bus_calculate_address_packet(address, 0)) != 0x18) {
		// ACK was not received
		bus_stop();
		return 2;
	}

	if (bus_write(byte1) != 0x28) {
		// ACK was not received
		bus_stop();
		return 3;
	}

	if (bus_write(byte2) != 0x28) {
		// ACK was not received
		bus_stop();
		return 4;
	}

	// XXX: This may prevent proper receive without interruption
	bus_stop();

	sei();

	return 0;
}

/**
 *	Receive two bytes of data from the given address over the bus
 *
 *	This function disables interrupts at the start and reenables then when done
 */
uint8_t bus_receive(uint8_t address, uint16_t* data) {
	uint8_t byte1;
	uint8_t byte2;

	// XXX: Are interrupts always enabled when this function is called
	cli();

	if (bus_start() != 0) {
		return 1;
	}

	if (bus_write(bus_calculate_address_packet(address, 1)) != 0x40) {
		bus_stop();
		return 2;
	}

	// Receive one byte, then tell sender we expect more bytes
	if (bus_read_ack(&byte1) != 0x50) {
		bus_stop();
		return 3;
	}

	// Receive one byte, then tell sender that was the last byte
	if (bus_read_nack(&byte2) != 0x58) {
		bus_stop();
		return 4;
	}

	bus_stop();

	*data = ((uint16_t)byte1 << 8) | (uint16_t)byte2;

	sei();

	return 0;
}

uint8_t should_receive_first_data() {
	return ((TWSR & 0xf8) == 0x60 || ((TWSR & 0xf8) == 0x68) || ((TWSR & 0xf8) == 0x70));
}

uint8_t should_receive_second_data() {
	return (((TWSR & 0xf8) == 0x80) || ((TWSR & 0xf8) == 0x90) || ((TWSR & 0xf8) == 0x98));
}

uint8_t should_send_data() {
	return (((TWSR & 0xf8) == 0xA8) ||((TWSR & 0xf8) == 0xB0) || ((TWSR & 0xf8) == 0xB8));
}

uint16_t data_to_send() {
	return 0xf00f;
}

ISR(TWI_vect) {
	if (should_receive_first_data()) { //Own SLA+W received, ACK returned
		bus_data = 0x0000;
		TWCR |= (1 << TWINT) | (1 << TWEA) | (0 << TWSTO);
	}
	else if (should_receive_second_data()) { //Data has been received
		bus_data = bus_data << 8;
		bus_data |= (uint16_t)TWDR;
		TWCR |= (1 << TWINT) | (1 << TWEA) | (0 << TWSTO);
	}
	else if ((TWSR & 0xf8) == 0xA0) { //Transmission done, save value.
		test_var_for_bus = bus_data;
	}
	else if (should_send_data()) {
		uint16_t data = data_to_send();
		TWDR = (uint8_t)(data >> 8);
		TWCR |= (1 << TWINT) | (0 << TWSTO) | (1 << TWEA); // Data will be transmitted, expecting ACK

		while (!(TWCR & (1 << TWINT)));

		if ((TWSR & 0xf8) == 0xB8) {
			//Ack was received

		TWDR = (uint8_t)data;
		TWCR |= (1 << TWINT) | (0 << TWSTO) | (0 << TWEA); //Last data will be transmitted, expecting NACK
		}
	}

	else if((TWSR & 0xf8) == 0xC8) {
		TWCR |= (1 << TWINT) | (1 << TWEA) | (0 << TWSTA) | (0 << TWSTO);
	}
}
