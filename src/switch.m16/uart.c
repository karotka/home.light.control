#include <avr/io.h>
#include "uart.h"
#include <stdlib.h>


void UART_init() {
    UBRRH = (uint8_t)(UBRR_VALUE >> 8);
    UBRRL = (uint8_t)UBRR_VALUE;

    // Set
    //UCSR0B = (1 << RXEN0);    // Enable receiver
    UCSRB = (1 << TXEN);    // Enable transmiter
    UCSRC = (1 << URSEL) | (1 << USBS) | (3 << UCSZ0);
    //UCSRB |= (1 << RXCIE0);
}

void UART_puti(const int val){
    char buffer[8];
    UART_puts(itoa(val, buffer, 10));
}

void UART_putc(unsigned char data) {
    while (!(UCSRA & (1 << UDRE)));       // Wait for empty transmit buffer
    UDR = data;                           // Start transmittion
}

unsigned char UART_getc (void) {
    while (!(UCSRA & (1 << RXC)));
    return UDR;
}

void UART_puts(char *str) {
    while(*str) {
        UART_putc(*str++);
    }
}
