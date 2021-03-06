#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <util/delay.h>
#include "uart.h"
#include "touch.h"
//#include "twi.h"


static touch_channel_t btn1 = {
    .mux = 0,
    .port = &PORTC,
    .portmask = (1 << PC0),
    .state = btnOff,
    .count = 0,
};
static touch_channel_t btn2 = {
    .mux = 1,
    .port = &PORTC,
    .portmask = (1 << PC1),
    .state = btnOff,
    .count = 0,
};
static touch_channel_t btn3 = {
    .mux = 2,
    .port = &PORTC,
    .portmask = (1 << PC2),
    .state = btnOff,
    .count = 0,
};
static touch_channel_t btn4 = {
    .mux = 3,
    .port = &PORTC,
    .portmask = (1 << PC3),
    .state = btnOff,
    .count = 0,
};
static touch_channel_t btn5 = {
    .mux = 4,
    .port = &PORTC,
    .portmask = (1 << PC4),
    .state = btnOff,
    .count = 0,
};
static touch_channel_t btn6 = {
    .mux = 5,
    .port = &PORTC,
    .portmask = (1 << PC5),
    .state = btnOff,
    .count = 0,
};

void watchDogInit(void);
void pinsInit(void);
void pinsInit(void);
void timer0Init(void);
//void readAddress(volatile uint8_t *port, volatile uint8_t *addr);

volatile uint8_t value = 0;
volatile uint8_t lastValue = 0;
volatile uint8_t sleepValue = 9;
volatile uint8_t count = 0;
volatile uint8_t sleep = 1;

void handleButtons(void) {
    btn_set(&btn1);
    btn_set(&btn2);
    btn_set(&btn3);
    btn_set(&btn4);
    btn_set(&btn6);

    if (btn1.state == btnOn) {
        if (btn2.state == btnOn) {
            value = 6;
            return;
        } else {
            value = 7;
            return;
        }
    }

    if (btn2.state == btnOn) {
        if (btn3.state == btnOn) {
            value = 4;
            return;
        } else {
            value = 5;
            return;
        }
    }

    if (btn3.state == btnOn) {
        if (btn4.state == btnOn) {
            value = 2;
            return;
        } else {
            value = 3;
            return;
        }
    }

    if (btn4.state == btnOn) {
        value = 1;
        return;
    }

    // handle off
    if (btn6.state == btnRelease) {
        value = 0;
        sleep = 1;
        return;
    }
}

void sleepMode(void) {
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);//SLEEP_MODE_PWR_DOWN);
    cli();
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_mode();
    sleep_disable();
    power_all_enable();
}

ISR(WDT_vect) {
    // hadle on
    btn_set(&btn5);

    if (btn5.state == btnRelease) {
        value = sleepValue;
        sleep = 0;
    }

    if (sleep == 0) {
        handleButtons();
    }
}

void timer0Init(void) {
    // Set the Timer Mode to CTC
    TIFR0 |= (1 << TOV0);
    TIMSK0 |= (1 << TOIE0);
    TCNT0 = 255;
    TCCR0B |= (1 << CS00) | (1 << CS02);
}

int main(void) {
    //volatile uint8_t TWI_slaveAddress;

    pinsInit();
    watchDogInit();
    //pwmInit();
    //timer0Init();

    //readAddress(&PINB, &TWI_slaveAddress);
    //TWI_slaveAddress = 0x20;

    _delay_ms(200);
    sei();

    // Initialise TWI module for slave operation
    //TWI_SlaveInitialise(TWI_slaveAddress << TWI_ADR_BITS);
    // Start the TWI transceiver
    //TWI_StartTransceiver();

    // Start UART
    UART_init();

    // Init ADC touch library
    TOUCH_init();

    //uint8_t temp, value;

    uint8_t i;
    for(;;) {
        if (lastValue != value) {
            for (i = 0; i < 4; i++) {
                PORTD |= (1 << PD4);
                UART_puti(value);
                _delay_ms(20);
                lastValue = value;
            }
            PORTD &= ~(1 << PD4);
            if (value != 0) {
                sleepValue = value;
            } else {
                sleepMode();
            }
        }
        //if (!TWI_TransceiverBusy()) {
        //    //PORTD |= (1 << PD7);
        //    if (TWI_statusReg.RxDataInBuf) {
        //        TWI_GetDataFromTransceiver(&temp, 1);
        //    }
        //    TWI_StartTransceiverWithData(&temp, 1);
        //}
        //PORTD ^= (1 << PD4);
        //UART_puts("ahoj");
        //UART_putc('\n');
    }
}

void watchDogInit(void) {
    // Watch dog every 0.25s and power down
    WDTCSR |= (1 << WDP2);
    WDTCSR |= (1 << WDIE);
}

void pinsInit(void) {
    DDRD = 0x00;
    PORTD = 0x00;
    DDRD |= (1 << PD4);

    DDRC = 0x00;
    PORTC = 0x00;

    // all B pins as input
    DDRB = 0x00;
    PORTB = 0x00;
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
