#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"
#include "TWI_slave.h"

// Sample TWI transmission commands
#define TWI_CMD_MASTER_WRITE 0x10
#define TWI_CMD_MASTER_READ  0x20


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
    //pwmInit();
    //timer2Init();

    unsigned char messageBuf[TWI_BUFFER_SIZE];
    unsigned char TWI_slaveAddress;

    // Own TWI slave address
    TWI_slaveAddress = 0x10;

    // Initialise TWI module for slave operation. Include address and/or enable General Call.
    TWI_Slave_Initialise((unsigned char)((TWI_slaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_GEN_BIT)));

    sei();

    TWI_Start_Transceiver();

    OCR0B = 0;

    //    char s[100];
    uint8_t temp;
    while (1) {

        if (!TWI_Transceiver_Busy()) {
            if (TWI_statusReg.RxDataInBuf) {
                TWI_Get_Data_From_Transceiver(messageBuf, 2);
                if (messageBuf[1] == TWI_CMD_MASTER_WRITE) {
                    value = messageBuf[0];
                    //sprintf (s, ": Data:%d CMD:%d  \n", messageBuf[0], messageBuf[1]); UART_puts(s);
                }
                if (messageBuf[1] == TWI_CMD_MASTER_READ) {
                    temp = value;
                    TWI_Start_Transceiver_With_Data(&temp, 1);
                    //sprintf (s, "Send: Data:%d CMD:%d \n", value, messageBuf[1]); UART_puts(s);
                }
            }
        }
        //_delay_ms(500);
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
