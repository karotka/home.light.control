#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

void pinsInit(void);
void pwmInit(void);

volatile uint8_t value;
volatile uint8_t data;
volatile int lastValue = 16;
volatile uint8_t incomingData[3];
volatile uint8_t incomingDataCounter = 0;
volatile uint8_t sendBack = 0;

const uint8_t dimmerSteps[17] = {
    0, 20, 25, 33,
    50, 80, 90, 100,
    110, 120, 130, 140,
    255, 255, 255, 255, 255
};

ISR(USART_RX_vect) {
    data = UDR;
    incomingData[incomingDataCounter] = data;

    if (incomingData[0] == incomingData[1] && incomingData[1] == incomingData[2]) {
        value = incomingData[0];
        UART_putc((char)value);
        PORTB ^= (1 << PB1);
    }

    incomingDataCounter++;
    if (incomingDataCounter == 3) {
        incomingDataCounter = 0;
    }
}

int main(void) {

    pinsInit();
    pwmInit();
    UART_init();
    sei();

    OCR0A = 0;

    while (1) {
        _delay_ms(100);
        OCR0A = dimmerSteps[(uint8_t)value];
        PORTB &= ~(1 << PB1);
    }

    return 0;
}

void pwmInit(void) {
    // fast PWM mode
    TCCR0A = (1 << COM0A0) | (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS00);   // clock source = CLK/8, start PWM
}

/**
 * DDR register IO register
 * 1 bit for output
 * 0 bit for input
 */
void pinsInit(void) {
    DDRB = (1 << PB2) | (1 << PB1);
    PORTB = 0x00;

    DDRD = (1 << PD2);
    PORTD &= ~(1 << PD2);
}
