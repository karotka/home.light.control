#include <avr/io.h>
#include "uart.h"
#include <stdlib.h>


void UART_init() {
    UBRRH = (uint8_t)(UBRR_VALUE >> 8);
    UBRRL = (uint8_t)UBRR_VALUE;

    // Set
    UCSRB = (1 << RXEN);    // Enable receiver
    //UCSRB = (1 << TXEN);    // Enable transmitter
    UCSRC = (1 << UCSZ0) | (1 << UCSZ1) | (1 << USBS);  // set to 8 data bits, 2 stop bit
    // Enable interupt by recieve UART byte
    UCSRB |= (1 << RXCIE);
}

void UART_puti(const int val) {
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
