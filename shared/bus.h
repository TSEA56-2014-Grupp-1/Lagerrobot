#ifndef BUS_H_
#define BUS_H_

void bus_init(uint8_t address);

uint8_t bus_send(uint8_t address, uint8_t byte1, uint8_t byte2);
uint8_t bus_receive(uint8_t address, uint16_t* data);

#endif /* BUS_H_ */
