#ifndef BUS_H_
#define BUS_H_

#define BUS_ADDRESS_CHASSIS 1
#define BUS_ADDRESS_ARM 6
#define BUS_ADDRESS_SENSOR 4
#define BUS_ADDRESS_COMMUNICATION 5

void bus_init(uint8_t address);

uint8_t bus_request(uint8_t , uint8_t, uint16_t, uint16_t*);
uint8_t bus_transmit(uint8_t, uint8_t, uint16_t);

uint8_t bus_register_response(uint8_t id, uint16_t (*callback)(uint8_t, uint16_t));
uint8_t bus_register_receive(uint8_t id, void (*callback)(uint8_t, uint16_t));

uint8_t bus_get_address(void);

#endif /* BUS_H_ */
