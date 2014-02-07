#define F_CPU 8000000UL
#define SENS 1000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "uart.h"
#include "touch.h"
#include "twi.h"

static touch_channel_t btn0 = {
    .mux = 0,
    .portmask = (1 << PC0)
};
static touch_channel_t btn1 = {
    .mux = 1,
    .portmask = (1 << PC1)
};
//static touch_channel_t btn2 = {
//    .mux = 2,
//    .port = &PORTC,
//    .portmask = (1 << PC2),
//};
//static touch_channel_t btn3 = {
//    .mux = 3,
//    .port = &PORTC,
//    .portmask = (1 << PC3),
//};

void pwmInit(void);
//void watchDogInit(void);
void pinsInit(void);
//void readAddress(volatile uint8_t *port, volatile uint8_t *addr);

int main(void) {
    volatile uint8_t TWI_slaveAddress;

    pinsInit();

    //watchDogInit();

    //pwmInit();

    //readAddress(&PINB, &TWI_slaveAddress);
    TWI_slaveAddress = 0x20;

    _delay_ms(200);

    // Initialise TWI module for slave operation
    TWI_SlaveInitialise(TWI_slaveAddress << TWI_ADR_BITS);

    sei();

    // Start the TWI transceiver
    TWI_StartTransceiver();

    // Start UART for debug
    UART_init();

    uint16_t sample0;
    uint16_t sample1;
    //uint16_t sample2;
    //uint16_t sample3;

    TOUCH_init();

    //uint8_t temp;

    for(;;) {
        //if (!TWI_TransceiverBusy()) {
        //    //PORTD |= (1 << PD7);
        //    if (TWI_statusReg.RxDataInBuf) {
        //        TWI_GetDataFromTransceiver(&temp, 1);
        //    }
        //    UART_putc(temp);
        //    UART_putc(TWI_slaveAddress);
        //    TWI_StartTransceiverWithData(&temp, 1);
        //    //OCR0A = temp;
        //}
        /*
        sample0 = touch_adc(&btn0);

        UART_puts("Value: ");
        UART_puti((uint16_t)sample0);
        UART_putc('\n');

        if (sample0 > 810) {
            PORTB |= (1 << PB7);
        } else {
            PORTB &= ~(1 << PB7); // set pullup off
        }
        */
        sample1 = touch_adc(&btn1);

        UART_puts("Value: ");
        UART_puti((uint16_t)sample1);
        UART_putc('\n');

        if (sample1 > 810) {
            PORTD |= (1 << PD7);
        } else {
            PORTD &= ~(1 << PD7); // set pullup off
        }

        _delay_ms(200);
        //_delay_us(32);

    }
}

void pwmInit(void) {
    TCCR0A = (1 << COM0A1) | (1 << WGM00);
    TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A1) | (0 << COM0A0);
    TCCR0B = (1 << CS01);
}

void watchDogInit(void) {
    // Watch dog every 0.25s and power down
    WDTCSR |= (1 << WDP2);// | (1 << WDP2);
    WDTCSR |= (1 << WDIE);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void pinsInit(void) {
    DDRD = 0x00;
    PORTD = 0x00;
    DDRD |= (1 << PD5) | (1 << PD6) | (1 << PD7);

    DDRC = 0x00;
    PORTC = 0x00;
    //DDRC |= (1 << PD0) | (1 << PD7);// | (1 << PD7);

    // all B pins as input
    DDRB = 0x00;
    PORTB = 0x00;
    DDRB |= (1 << PB7);
}

void readAddress(volatile uint8_t *port, volatile uint8_t *addr) {

    if(bit_is_set(*port, PB0)) {
        *addr = (1 << 0);
    }
    if(bit_is_set(*port, PB1)) {
        *addr |= (1 << 1);
    }
    if(bit_is_set(*port, PB2)) {
        *addr |= (1 << 2);
    }
    if(bit_is_set(*port, PB3)) {
        *addr |= (1 << 3);
    }
    if(bit_is_set(*port, PB4)) {
        *addr |= (1 << 4);
    }
    if(bit_is_set(*port, PB5)) {
        *addr |= (1 << 5);
    }
    if(bit_is_set(*port, PB6)) {
        *addr |= (1 << 6);
    }
    if(bit_is_set(*port, PB7)) {
        *addr |= (1 << 7);
    }
}
