/**
 *	Base library for bus communication
 *
 *	@author Andreas Runfalk & Patrik Nyberg
 */

#include <avr/io.h>
#include <avr/interrupt.h>

/**
 *	Temporary storage for received data
 */
uint16_t bus_data;

/**
 *	Temporary storage for response data
 */
uint16_t bus_response_data;

/**
 *	Array of response callbacks used by bus_register_response() and bus_call_response()
 */
uint16_t (*response_callbacks[64])(uint8_t, uint16_t) = {0};

/**
 *	Array of receive callbacks used by bus_register_receive() and bus_call_receive()
 */
void (*receive_callbacks[64])(uint8_t, uint16_t) = {0};

/**
 *	Start listening on given address and enable interrupts
 *
 *	@param address 7-bit address to listen to
 */
void bus_init(uint8_t address) {
	// Modify bus frequency
	// 18200000/(16+2*TWBR*(4^TWSR prescaler)) = 8.8 kHz (testing shows 15.5kHz ...)
	// http://www.avrbeginners.net/architecture/twi/twi.html
	TWBR = 0x80;
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
 *	Try to acquire control over the bus. Will continuously retry until control is
 *	aquired. Possible status codes are:
 *
 *	- 0 if bytes were successfully sent
 *
 *	@return Status code
 */
uint8_t bus_start() {
	uint8_t status_code;
	cli();
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);

	// Wait for TWINT to go high
	while (!(TWCR & (1 << TWINT)));

	status_code = TWSR & 0xf8;
	sei();

	// Check if failure to seize bus
	switch (status_code) {
		case 0x08: // Start
		case 0x10: // Repeated start
			return 0;
		case 0x00:
			cli();
			TWCR &= 0xff ^ (1 << TWSTA);
			TWCR |= (1 << TWINT) | (1 << TWSTO);
			while (!(TWCR & (1 << TWINT)));
			sei();
		case 0xf8:
		default: 
			// Catastrophic failure, retry
			return bus_start();
	}
}

/**
 *	Let go of the control over the buss
 */
void bus_stop() {
	TWCR |= (1 << TWINT) | (1 << TWSTO) | (1 << TWEA);
}

/**
 *	Write a byte of data on the bus
 *
 *	@param data Data byte to write
 *
 *	@return Masked status code from TWSR
 */
uint8_t bus_write(uint8_t data) {
	uint8_t status_code;

	cli();
	TWDR = data;
	TWCR &= 0xff ^ (1 << TWSTA) ^ (1 << TWSTO);
	TWCR |= (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	// Wait for TWINT to go high (package sent)
	while (!(TWCR & (1 << TWINT)));

	status_code = TWSR & 0xf8;
	sei();

	return status_code;
}

/**
 *	Read one byte from the bus and send ACK to indicate more packets are expected.
 *
 *	@param &data Reference to uint8_t variable where data should be saved
 *
 *	@return Masked status code from TWSR
 */
uint8_t bus_read_ack(uint8_t* data) {
	uint8_t status_code;

	cli();

	TWCR |= (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

	// Wait for TWINT to go high
	while (!(TWCR & (1 << TWINT)));

	*data = TWDR;

	status_code = TWSR & 0xf8;
	sei();

	return status_code;
}

/**
 *	Read one byte from the bus and send NACK to indicate no more packets are
 *	expected.
 *
 *	@param &data Reference to uint8_t variable where data should be saved
 *
 *	@return Masked status code from TWSR
 */
uint8_t bus_read_nack(uint8_t* data) {
	uint8_t status_code;

	cli();

	TWCR &= 0xff ^ (1 << TWEA);
	TWCR |= (1 << TWINT) | (1 << TWEN);

	// Wait for TWINT to go high
	while (!(TWCR & (1 << TWINT)));

	*data = TWDR;

	status_code = TWSR & 0xf8;
	sei();

	return status_code;
}

/**
 *	Calculate data byte for addressing the bus
 *
 *	@param address 7-bit address for target, the 7th bit is ignored
 *	@param direction 1-bit to set send or receive, only the 0th bit is used. 0 means send, 1 means receive
 *
 *	@return Merged 7-bit address and direction bit
 */
uint8_t bus_calculate_address_packet(uint8_t address, uint8_t direction) {
	if (direction) {
		return (address << 1) | 1;
	} else {
		return (address << 1) & 0xfe;
	}
}

/**
 *	Send two bytes of raw data over the bus. Possible status codes are:
 *
 *	- 0 if bytes were successfully sent
 *	- 1 if failure to start bus communication
 *	- 2 if target does not respond
 *	- 3 if first byte was written but not acknowledged
 *	- 4 if second byte was written but not acknowledged
 *
 *	@param address Target unit address
 *	@param byte1 First byte to send
 *	@param byte2 Second byte to send
 *
 *	@return Transfer status code
 */
uint8_t bus_send(uint8_t address, uint8_t byte1, uint8_t byte2) {
	if (bus_start() != 0) {
		return 1;
	}

	switch (bus_write(bus_calculate_address_packet(address, 0))) {
		case 0x18: // Successfully addressed target
			break;
		case 0x20: // Target does not respond
			bus_stop();
			return 2;
		case 0x38: // Arbitration lost
		default: // Catastrophic failure
			return bus_send(address, byte1, byte2);
	}

	switch (bus_write(byte1)) {
		case 0x28: // Byte transmitted and ACK received
			break;
		case 0x30: // Byte transmitted but NACK received
			bus_stop();
			return 3;
		case 0x38: // Arbitration lost
		default: // Catastrophic failure
			return bus_send(address, byte1, byte2);
	}

	switch (bus_write(byte2)) {
		case 0x28: // Byte transmitted and ACK received
			break;
		case 0x30: // Byte transmitted but NACK received
			bus_stop();
			return 4;
		case 0x38: // Arbitration lost
		default: // Catastrophic failure
			return bus_send(address, byte1, byte2);
	}

	return 0;
}

/**
 *	Receive two bytes of data from the given address over the bus. Possible
 *	status codes are:
 *
 *	- 0 if bytes were successfully received
 *	- 1 if failure to start bus communication
 *	- 2 if request was transmitted but was not acknowledged by destination
 *	- 3 if first byte couldn't be received
 *	- 4 if second byte couldn't be received
 *
 *	@param address Target address
 *	@param &data Reference to uint16_t variable where data should be saved
 *
 *	@return Transfer status code
 */
uint8_t bus_receive(uint8_t address, uint16_t* data) {
	uint8_t byte1;
	uint8_t byte2;

	if (bus_start() != 0) {
		return 1;
	}

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

	bus_stop();

	*data = ((uint16_t)byte1 << 8) | (uint16_t)byte2;

	return 0;
}

/**
 *	Extract 5-bit ID from a packet sent by bus_request() or bus_transmit()
 *
 *	@param data Raw 16-bit data from the bus
 *
 *	@return 8-bit representation 5-bit address left padded with zeros
 */
uint8_t bus_translate_id(uint16_t data) {
	return (uint8_t)(data >> 11);
}

/**
 *	Extract 11-bit metadata from a packet sent by bus_request or bus_transmit
 *
 *	@param data Raw 16-bit data from the bus
 *
 *	@return 16-bit representation of 11-bit metadata left padded with zeros
 */
uint16_t bus_translate_metadata(uint16_t data) {
	return data & 0x07ff;
}

/**
 *	Transmits 11 bits of data to the target address which passes it to the
 *	callback function with the given ID.
 *
 *	@param address Target address
 *	@param id ID of receive callback on target
 *	@param data 11-bits to transfer
 *
 *	@return See bus_send()
 */
uint8_t bus_transmit(uint8_t address, uint8_t id, uint16_t data) {
	uint8_t status_code = bus_send(
		address,
		(id << 3) | (0x07 & (uint8_t)(data >> 8)),
		(uint8_t)data);
	bus_stop();

	return status_code;
}

/**
 *	Transmits 11 bytes of metadata to the target address which is handled by the
 *	callback with the given ID. The callback returns two bytes of data. If the
 *	target does not have a callback with this ID two zero bytes are returned.
 *
 *	@param address Target address
 *	@param id ID of response callback on target
 *	@param metadata 11-bit metadata that is passed to the response callback
 *	@param &received_data Reference to uint16_t variable where response data should be saved
 *
 *	@return
 *		Top 4 bits contains status code by bus_receive() and lower 4 bits contain
 *		status code returned by bus_send()
 */
int8_t bus_request(uint8_t address, uint8_t id, uint16_t metadata, uint16_t* received_data) {
	int8_t send_status;

	send_status = bus_send(
		address,
		(id << 3) | (0x07 & (uint8_t)(metadata >> 8)),
		(uint8_t)metadata);
	if (send_status != 0) {
		return send_status;
	}

	send_status = bus_receive(address, received_data);
	if (send_status != 0) {
		return send_status << 4;
	}

	return 0;
}

/**
 *	Checks if there is a registered response callback for this ID
 *
 *	@param id Callback ID to test for
 *
 *	@return 1 if callback exists 0 otherwise
 */
uint8_t bus_has_response(uint8_t id) {
	return response_callbacks[id] != 0;
}

/**
 *	Checks if there is a registered receive callback for this ID
 *
 *	@param id Callback ID to test for
 *
 *	@return 1 if callback exists 0 otherwise
 */
uint8_t bus_has_receive(uint8_t id) {
	return receive_callbacks[id] != 0;
}

/**
 *	Register a handler to reply to bus_requests. The ID is shared with
 *	bus_register_receive(). Max ID is 63.
 *
 *	- 0 if successful
 *	- 1 if ID is already bound by either bus_register_response() or bus_register_receive()
 *	- 2 if ID is greater than 63
 *
 *	@param id ID to listen to
 *	@param callback Callback function
 *
 *	@return Status code
 */
uint8_t bus_register_response(uint8_t id, uint16_t (*callback)(uint8_t, uint16_t)) {
	if (bus_has_response(id) || bus_has_receive(id)) {
		return 1;
	}

	if (id > 63) {
		return 2;
	}

	response_callbacks[id] = callback;

	return 0;
}

/**
 *	Register a handler to handle bus_tranmit(). The ID is shared with
 *	bus_register_response. Max ID is 63. Possible status codes are:
 *
 *	- 0 if successful
 *	- 1 if ID is already bound by either bus_register_response() or bus_register_receive()
 *	- 2 if ID is greater than 63
 *
 *	@param id ID to listen to
 *	@param callback Callback function
 *
 *	@return Status code
 */
uint8_t bus_register_receive(uint8_t id, void (*callback)(uint8_t, uint16_t)) {
	if (bus_has_response(id) || bus_has_receive(id)) {
		return 1;
	}

	if (id > 63) {
		return 2;
	}

	receive_callbacks[id] = callback;

	return 0;
}

/**
 *	Calls the response callback with the given ID and provides it the given data
 *
 *	@param id 5-bit callback ID registered by bus_register_response()
 *	@param data 11-bit data to provide callback
 *
 *	@return 0 if there is no response with given ID, else callback return data
 */
uint16_t bus_call_response(uint8_t id, uint16_t data) {
	if (!bus_has_response(id)) {
		return 0;
	}
	return (*response_callbacks[id])(id, data);
}

/**
 *	Calls the receive callback with the given ID and provides it the given data
 *
 *	@param id 5-bit callback ID registered by bus_register_response()
 *	@param data 11-bit data to provide callback
 */
void bus_call_receive(uint8_t id, uint16_t data) {
	if (!bus_has_receive(id)) {
		return;
	}
	(*receive_callbacks[id])(id, data);
}

/**
 *  Returns the address the unit is configured to listen on
 */
uint8_t bus_get_address() {
	return TWAR >> 1;
}

/**
 *	Handle interrupts from other masters
 */
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
			TWCR |= (1 << TWINT) | (1 << TWEA);
			bus_call_receive(
				bus_translate_id(bus_data), bus_translate_metadata(bus_data));
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
