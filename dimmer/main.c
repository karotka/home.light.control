#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "uart.h"

void pwmInit(void);
void pinsInit(void);

volatile uint8_t data;

ISR(USART_RX_vect) {
    data = UDR;
    //PORTD ^= (1 << PD6);
}

uint8_t receive_uart() {
    while (!(UCSRA & (1 << RXC)));
    return UDR;
}

int main(void) {
    UART_init();
    pinsInit();
    pwmInit();
    sei();
    PORTB &= ~(1 << PB3);
    OCR0A = 0;

    for(;;) {

        _delay_ms(10);
        //if (data == 49) {
        //}
        if (data == 48) {
            PORTB &= ~(1 << PB3);
            OCR0A = 255;
        }
        else if (data == 49) {
            PORTB |= (1 << PB3);
            OCR0A = 230;
        }
        else if (data == 50) {
            PORTB |= (1 << PB3);
            OCR0A = 190;
        }
        else if (data == 51) {
            PORTB |= (1 << PB3);
            OCR0A = 160;
        }
        else if (data == 52) {
            PORTB |= (1 << PB3);
            OCR0A = 110;
        }
        else if (data == 53) {
            PORTB |= (1 << PB3);
            OCR0A = 80;
        }
        else if (data == 54) {
            PORTB |= (1 << PB3);
            OCR0A = 60;
        }
        else if (data == 55) {
            PORTB |= (1 << PB3);
            OCR0A = 30;
        }
        else if (data == 57) {
            PORTB |= (1 << PB3);
            OCR0A = 0;
            PORTB &= ~(1 << PB2);
        }
    }
}

void pwmInit(void) {
    // fast PWM mode
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS00);   // clock source = CLK/8, start PWM
}

void pinsInit(void) {
    DDRB = 0x00;
    DDRD = 0x00;
    DDRB |= (1 << PB2) | (1 << PB3);
    DDRD |= (1 << PD6);
}
