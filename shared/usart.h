#ifndef USART_H_
#define USART_H_

#include <avr/io.h>
//#define USART_RECEIVE_TIMEOUT_COUNT 60000

void usart_init(uint16_t baudrate_register_value);
void usart_write_byte(uint8_t data);
uint8_t usart_read_byte(uint8_t * data);

uint8_t usart_has_bytes();
uint8_t usart_ready_to_write();
uint8_t usart_tx_complete();

#endif