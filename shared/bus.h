#ifndef BUS_H_
#define BUS_H_

void bus_init(uint8_t address);

uint8_t bus_send(uint8_t, uint8_t, uint8_t);
uint8_t bus_receive(uint8_t, uint16_t*);

void bus_request(uint8_t , uint8_t, uint16_t, uint16_t*);
void bus_transmit(uint8_t, uint8_t, uint8_t);

int8_t bus_translate_id(uint16_t);
uint16_t bus_translate_metadata(uint16_t);

uint8_t bus_register_response(uint8_t id, uint16_t (*callback)(uint16_t));
uint16_t bus_call_response(uint8_t, uint16_t);

#endif /* BUS_H_ */