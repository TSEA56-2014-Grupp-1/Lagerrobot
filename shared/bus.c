#include <avr/io.h>
#include <avr/interrupt.h>

uint16_t bus_data;
uint16_t bus_response_data;


// Global for storing registered callback response functions. Default value is NULL
uint16_t (*response_callbacks[64])(uint8_t, uint16_t) = {0};

void bus_init(uint8_t address) {
	// Modify bus frequency
	// 18200000/(16+2*TWBR*(4^TWSR prescaler)) = 8.8 kHz (testing shows 15.5kHz ...)
	// http://www.avrbeginners.net/architecture/twi/twi.html
	TWBR = 0xff;
	//TWBR = 0x0D; // Works good (~192kHz)

	// Enable internal pull-up for SDA and SCL
	PORTC |= 0x03;

	// Set address on bus
	TWAR = (address << 1) | 1;

	// Enable the bus
	TWSR &= 0xfc;
	TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE);

	sei();
}

/**
 *	Try to acquire control over the bus
 */
uint8_t bus_start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);

	// Wait for TWINT to go high
	while (!(TWCR & (1 << TWINT)));

	// Check if failure to seize bus
	switch (TWSR & 0xf8) {
		case 0x08: // Start
		case 0x10: // Repeated start
			return 0;
		case 0x00:
			TWCR &= 0xff ^ (1 << TWSTA);
			TWCR |= (1 << TWINT) | (1 << TWSTO);
		case 0xf8:
		default:
			// Catastrophic failure, retry
			return bus_start();
	}
}

/**
 *	Let go of the control over the buss
 */
void bus_stop(void) {
	TWCR |= (1 << TWINT) | (1 << TWSTO) | (1 << TWEA);
}

/**
 *	Write a byte of data on the bus
 */
uint8_t bus_write(uint8_t data) {
	TWDR = data;
	TWCR &= 0xff ^ (1 << TWSTA) ^ (1 << TWSTO);
	TWCR |= (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	// Wait for TWINT to go high (package sent)
	while (!(TWCR & (1 << TWINT)));

	return TWSR & 0xf8;
}

/**
 *	Read one byte from the bus and send ACK to indicate we expect more packets
 */
uint8_t bus_read_ack(uint8_t* data) {
	TWCR |= (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

	// Wait for TWINT to go high
	while (!(TWCR & (1 << TWINT)));

	*data = TWDR;

	return TWSR & 0xf8;
}

/**
 *	Read one byte from the bus and send NACK to indicate we expect no more packets
 */
uint8_t bus_read_nack(uint8_t* data) {
	TWCR &= 0xff ^ (1 << TWEA);
	TWCR |= (1 << TWINT) | (1 << TWEN);

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
 *	This function disables interrupts at the start and re-enables then when done
 */
uint8_t bus_send(uint8_t address, uint8_t byte1, uint8_t byte2) {
	switch (bus_write(bus_calculate_address_packet(address, 0))) {
		case 0x18: // Successfully addressed target
			break;
		case 0x20: // Target does not respond
			bus_stop();
			return 2;
		case 0x38: // Arbitration lost
			return bus_send(address, byte1, byte2);
		default: // Catastrophic failure
			bus_stop();
			return 3;
	}

	switch (bus_write(byte1)) {
		case 0x28: // Byte transmitted and ACK received
			break;
		case 0x30: // Byte transmitted but NACK received
			bus_stop();
			return 4;
		case 0x38: // Arbitration lost
			return bus_send(address, byte1, byte2);
		default: // Catastrophic failure
			bus_stop();
			return 5;
	}

	switch (bus_write(byte2)) {
		case 0x28: // Byte transmitted and ACK received
			break;
		case 0x30: // Byte transmitted but NACK received
			bus_stop();
			return 6;
		case 0x38: // Arbitration lost
			return bus_send(address, byte1, byte2);
		default: // Catastrophic failure
			bus_stop();
			return 7;
	}

	return 0;
}

/**
 *	Receive two bytes of data from the given address over the bus
 *
 *	This function disables interrupts at the start and re-enables then when done
 */
uint8_t bus_receive(uint8_t address, uint16_t* data) {
	uint8_t byte1;
	uint8_t byte2;

	switch (bus_write(bus_calculate_address_packet(address, 1))) {
		case 0x38: // Arbitration lost
			return bus_receive(address, data);
		case 0x40: // Request transmitted and ACK received
			break;
		case 0x48: // Request transmitted and NACK received
		default: // Catastrophic failure
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

	*data = ((uint16_t)byte1 << 8) | (uint16_t)byte2;

	return 0;
}

int8_t bus_translate_id(uint16_t data) {
	return (uint8_t)(data >> 11);
}

int16_t bus_translate_metadata(uint16_t data) {
	return data & 0x07ff;
}

int8_t bus_transmit(uint8_t address, uint8_t byte1, uint8_t byte2) {
	if (bus_start() != 0) {
		return 1;
	}

	cli();
	int8_t status_code = bus_send(address, byte1, byte2);
	sei();

	bus_stop();

	return status_code;
}

int8_t bus_request(uint8_t address, uint8_t id, uint16_t metadata, uint16_t* received_data) {
	int8_t send_status;

	if (bus_start() != 0) {
		return 1;
	}

	cli();

	send_status = bus_send(
		address,
		(id << 3) | (0x07 & (uint8_t)(metadata >> 8)),
		(uint8_t)metadata);

	if (send_status != 0) {
		sei();
		return send_status;
	}

	if (bus_start() != 0) {
		sei();
		return 1;
	}

	send_status = bus_receive(address, received_data);
	if (send_status != 0) {
		sei();
		return send_status << 4;
	}

	sei();

	bus_stop();

	return 0;
}

uint8_t bus_register_response(uint8_t id, uint16_t (*callback)(uint8_t, uint16_t)) {
	if (response_callbacks[id] != 0) {
		return 1;
	}

	if (id > 63) {
		return 2;
	}

	response_callbacks[id] = callback;

	return 0;
}

uint16_t bus_call_response(uint8_t id, uint16_t data) {
	return (*response_callbacks[id])(id, data);
}

ISR(TWI_vect) {
	switch (TWSR & 0xf8) {
		// Slave receive
		case 0x60: // SLA+W received and ACK returned
		case 0x68: // Arbitration lost, but SLA+W received and ACK returned
		case 0x70: // General call received and ACK returned
		case 0x78: // Arbitration lost, but general call received and ACK returned
			bus_data = 0x0000;
			TWCR |= (1 << TWINT) | (1 << TWEA);
			break;

		case 0x80: // Data byte received and ACK returned
		case 0x90: // General call, data byte received and ACK returned
			// This gets called every time data is received so data is shifted
			// before adding
			bus_data = bus_data << 8;
			bus_data |= (uint16_t)TWDR;

			TWCR |= (1 << TWINT) | (1 << TWEA);
			break;

		case 0x88: // Data byte received and NACK returned
		case 0x98: // General call, data byte received and NACK returned
			bus_data = 0x0000;
			TWCR |= (1 << TWINT) | (1 << TWEA);
			break;

		case 0xa0: // Stop or repeated start received
			// TODO: Call function to handle received data
			TWCR |= (1 << TWINT) | (1 << TWEA);
			break;

		// Slave transmit
		case 0xa8: // SLA+R received and ACK returned
		case 0xb0: // Arbitration lost but SLA+R received and ACK returned
			bus_response_data = bus_call_response(
				bus_translate_id(bus_data), bus_translate_metadata(bus_data));

			TWDR = (uint8_t)(bus_response_data >> 8);
			TWCR |= (1 << TWINT) | (1 << TWEA);
			break;

		case 0xb8: // Data byte transmitted and ACK received
			TWDR = (uint8_t)bus_response_data;
			TWCR &= 0xff ^ (1 << TWEA) ^ (1 << TWSTO);
			TWCR |= (1 << TWINT);
			break;

		case 0xc0: // Data byte transmitted and NACK received
		case 0xc8: // Last byte transmitted but ACK received
			TWCR &= 0xff ^ (1 << TWSTO);
			TWCR |= (1 << TWINT) | (1 << TWEA);
			break;
	}
}
