#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

void pwmInit(void);
void pinsInit(void);

volatile uint8_t value;
volatile uint8_t data;
volatile uint8_t lastData;

uint8_t vals[17] = {
    255, 230, 225, 220,
    210, 207, 205, 200,
    160, 150, 130, 120,
    0, 0, 0, 0, 0
};

ISR(USART_RX_vect) {
    data = UDR0;
    //UART_putc(data);

    if (data == lastData) {
        value = (int)data;
    } else {
        lastData = data;
    }
}

int main(void) {

    UART_init();
    pinsInit();
    pwmInit();
    sei();
    OCR0B = 0;

    while (1) {
        OCR0B = vals[value];
        //UART_putc(OCR0B);
        _delay_ms(200);
    }

    return 0;
}

void pwmInit(void) {
    TCCR0A = (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS00);   // clock source = CLK/8, start PWM
}

void pinsInit(void) {
    DDRC = 0x00;
    PORTC = 0x00;
    DDRC |= (1 << PC1);
    // low - enable receiver
    PORTC &= ~(1 << PC1);

    DDRD = 0x00;
    PORTD = 0x00;
    DDRD |= (1 << PD5);
}
