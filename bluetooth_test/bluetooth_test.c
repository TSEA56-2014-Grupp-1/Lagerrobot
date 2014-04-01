/*
 * bluetooth_test.c
 *
 * Created: 2014-04-01 11:41:25
 *  Author: Karl
 */ 

#define F_CPU 18.432E6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(USART0_RX_vect) {
	char data;
	data = UDR0;
	
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
	while (!(UCSR0A & (1 << TXC0)));
}

int main(void)
{
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	UCSR0C =  (3 << UCSZ00);
	UBRR0H = 0x00;
	UBRR0L = 0x09;
	
	UDR0 = 'A';
	_delay_ms(100);
	UDR0 = 0x20;
	
	sei();
	
    while(1)
    {
        
    }
}