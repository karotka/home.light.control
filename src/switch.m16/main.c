#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <util/delay.h>
#include "uart.h"
#include "touch.h"
//#include "twi.h"


static touch_channel_t btn0 = {
    .mux = 0,
    .port = &PORTA,
    .portmask = (1 << PA0),
    .state = btnOff,
    .limit = 150,
};
static touch_channel_t btn1 = {
    .mux = 1,
    .port = &PORTA,
    .portmask = (1 << PA1),
    .state = btnOff,
    .limit = 100,
};
static touch_channel_t btn2 = {
    .mux = 2,
    .port = &PORTA,
    .portmask = (1 << PA2),
    .state = btnOff,
    .limit = 100,
};
static touch_channel_t btn3 = {
    .mux = 3,
    .port = &PORTA,
    .portmask = (1 << PA3),
    .state = btnOff,
    .limit = 100,
};
static touch_channel_t btn4 = {
    .mux = 4,
    .port = &PORTA,
    .portmask = (1 << PA4),
    .state = btnOff,
    .limit = 100,
};
static touch_channel_t btn5 = {
    .mux = 5,
    .port = &PORTA,
    .portmask = (1 << PA5),
    .state = btnOff,
    .limit = 100,
};
static touch_channel_t btn6 = {
    .mux = 6,
    .port = &PORTA,
    .portmask = (1 << PA6),
    .state = btnOff,
    .limit = 100,
};
static touch_channel_t btn7 = {
    .mux = 7,
    .port = &PORTA,
    .portmask = (1 << PA7),
    .state = btnOff,
    .limit = 130,
};


void watchDogInit(void);
void pinsInit(void);
void pinsInit(void);
void timer0Init(void);
//void readAddress(volatile uint8_t *port, volatile uint8_t *addr);

volatile uint8_t value = 0;
volatile uint8_t lastValue = 0;

void handleButtons(void) {
    btn_set(&btn0);
    btn_set(&btn1);
    btn_set(&btn2);
    btn_set(&btn3);
    btn_set(&btn4);
    btn_set(&btn5);
    btn_set(&btn6);
    btn_set(&btn7);

    if (btn1.state == btnOn) {
        if (btn2.state == btnOn) {
            value = 2;
            return;
        } else {
            value = 1;
            return;
        }
    }

    if (btn2.state == btnOn) {
        if (btn3.state == btnOn) {
            value = 4;
            return;
        } else {
            value = 3;
            return;
        }
    }

    if (btn3.state == btnOn) {
        if (btn4.state == btnOn) {
            value = 6;
            return;
        } else {
            value = 5;
            return;
        }
    }

    if (btn4.state == btnOn) {
        if (btn5.state == btnOn) {
            value = 8;
            return;
        } else {
            value = 7;
            return;
        }
    }

    if (btn5.state == btnOn) {
        if (btn6.state == btnOn) {
            value = 10;
            return;
        } else {
            value = 9;
            return;
        }
    }

    if (btn6.state == btnOn) {
        value = 11;
        return;
    }

    if (btn7.state == btnOn) {
        value = 16;
        return;
    }

    // handle off
    if (btn0.state == btnRelease) {
        value = 0;
        return;
    }
}

ISR(TIMER0_OVF_vect) {
    handleButtons();
}

void timer0Init(void) {
    // Set the Timer Mode to OVF
    TIMSK |= (1 << TOIE0);
    TCCR0 |= (1 << CS00) | (1 << CS02); // / 1024
}

int main(void) {
    //volatile uint8_t TWI_slaveAddress;

    pinsInit();
    timer0Init();

    //readAddress(&PINB, &TWI_slaveAddress);
    //TWI_slaveAddress = 0x20;

    _delay_ms(200);
    sei();

    // Start UART
    UART_init();

    // Init ADC touch library
    TOUCH_init();

    uint8_t i;
    for(;;) {

        if (lastValue != value) {
            for (i = 0; i < 4; i++) {
                PORTB ^= (1 << PB4);
                UART_putc((char)value);
                _delay_ms(20);
                lastValue = value;
            }
        }
        if (value == 0) {
            PORTD |= (1 << PD3) | (1 << PD4);
        } else {
            PORTD &= ~(1 << PD3);
            PORTD &= ~(1 << PD4);
        }

        //if (!TWI_TransceiverBusy()) {
        //    //PORTD |= (1 << PD7);
        //    if (TWI_statusReg.RxDataInBuf) {
        //        TWI_GetDataFromTransceiver(&temp, 1);
        //    }
        //    TWI_StartTransceiverWithData(&temp, 1);
        //}
    }
}

void pinsInit(void) {
    // all B pins as input
    DDRB = 0x00;
    PORTB = 0x00;
    DDRB |= (1 << PB4);

    DDRC = 0x00;
    PORTC = 0x00;

    DDRD = 0x00;
    PORTD = 0x00;

    DDRD  |= (1 << PD3) | (1 << PD4);
    DDRD  |= (1 << PD2);
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
