#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

void pinsInit(void);
void pwmInit(void);
void timer2Init(void);

volatile uint8_t value;
volatile uint8_t lastValue;
volatile uint8_t data;
volatile uint8_t lastData;
volatile uint8_t incomingData[3];
volatile uint8_t incomingDataCounter = 0;
volatile uint8_t sendBack = 0;

uint8_t dimmerSteps[17] = {
    255, 230, 225, 220,
    210, 207, 205, 200,
    160, 150, 130, 120,
    0, 0, 0, 0, 0
};

ISR(USART_RX_vect) {
    data = UDR0;
    //UART_putc(data);

    incomingData[incomingDataCounter] = data;
    incomingDataCounter++;

    if (incomingDataCounter == 3) {
        incomingDataCounter = 0;
    }

    if (incomingData[0] == incomingData[1] && incomingData[1] == incomingData[2]) {
        value = (int)data;
        sendBack = 1;
    }

    //if (data == lastData) {
    //    value = (int)data;
    //} else {
    //    lastData = data;
    //}
}

ISR(TIMER2_OVF_vect) {
    if (sendBack == 0) {
        // high - enable trancseiver
        PORTC |= (1 << PC1);
        uint8_t i = 0;
        for (i = 0; i < 4; i++) {
            UART_putc((char)value);
            _delay_ms(20);
        }
        PORTC &= ~(1 << PC1);
        sendBack = 0;
    }
}

int main(void) {

    UART_init();
    pinsInit();
    pwmInit();
    timer2Init();

    sei();

    OCR0B = 0;

    while (1) {
        OCR0B = dimmerSteps[value];
    }

    return 0;
}

void pwmInit(void) {
    TCCR0A = (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS00);   // clock source = CLK/8, start PWM
}

void timer2Init(void) {
    // Set the Timer Mode to OVF
    TIMSK2 |= (1 << TOIE2);
    //TCCR0A |=
    TCCR2B |= (1 << CS20) | (1 << CS22); // / 1024
}

void pinsInit(void) {
    DDRC = 0x00;
    PORTC = 0x00;
    DDRC |= (1 << PC1);

    // low - enable receiver as default
    PORTC &= ~(1 << PC1);

    DDRD = 0x00;
    PORTD = 0x00;
    DDRD |= (1 << PD5);
}
