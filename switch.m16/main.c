#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"
#include "touch.h"
//#include "twi.h"


static touch_channel_t btn0 = {
    .mux = 0,
    .port = &PORTA,
    .portmask = (1 << PA0),
    .state = btnOff,
    .limit = 170,
};
static touch_channel_t btn1 = {
    .mux = 1,
    .port = &PORTA,
    .portmask = (1 << PA1),
    .state = btnOff,
    .limit = 90,
};
static touch_channel_t btn2 = {
    .mux = 2,
    .port = &PORTA,
    .portmask = (1 << PA2),
    .state = btnOff,
    .limit = 90,
};
static touch_channel_t btn3 = {
    .mux = 3,
    .port = &PORTA,
    .portmask = (1 << PA3),
    .state = btnOff,
    .limit = 90,
};
static touch_channel_t btn4 = {
    .mux = 4,
    .port = &PORTA,
    .portmask = (1 << PA4),
    .state = btnOff,
    .limit = 90,
};
static touch_channel_t btn5 = {
    .mux = 5,
    .port = &PORTA,
    .portmask = (1 << PA5),
    .state = btnOff,
    .limit = 90,
};
static touch_channel_t btn6 = {
    .mux = 6,
    .port = &PORTA,
    .portmask = (1 << PA6),
    .state = btnOff,
    .limit = 90,
};
static touch_channel_t btn7 = {
    .mux = 7,
    .port = &PORTA,
    .portmask = (1 << PA7),
    .state = btnOff,
    .limit = 170,
};


void watchDogInit(void);
void pinsInit(void);
void pinsInit(void);
void timer0Init(void);

volatile uint8_t value = 0;
volatile uint8_t send = 0;

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
        send = 1;
        if (btn2.state == btnOn) {
            value = 2;
            return;
        } else {
            value = 1;
            return;
        }
    }

    if (btn2.state == btnOn) {
        send = 1;
        if (btn3.state == btnOn) {
            value = 4;
            return;
        } else {
            value = 3;
            return;
        }
    }

    if (btn3.state == btnOn) {
        send = 1;
        if (btn4.state == btnOn) {
            value = 6;
            return;
        } else {
            value = 5;
            return;
        }
    }

    if (btn4.state == btnOn) {
        send = 1;
        if (btn5.state == btnOn) {
            value = 8;
            return;
        } else {
            value = 7;
            return;
        }
    }

    if (btn5.state == btnOn) {
        send = 1;
        if (btn6.state == btnOn) {
            value = 10;
            return;
        } else {
            value = 9;
            return;
        }
    }

    if (btn6.state == btnOn) {
        send = 1;
        value = 11;
        return;
    }

    if (btn7.state == btnOn) {
        send = 1;
        value = 16;
        return;
    }

    // handle off
    if (btn0.state == btnRelease) {
        send = 1;
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

    pinsInit();
    timer0Init();

    _delay_ms(200);
    sei();

    // Start UART
    UART_init();

    // Init ADC touch library
    TOUCH_init();

    uint8_t i;

    //char chr[100];
    //uint16_t val;

    for(;;) {

        //sprintf (chr, "0: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d \n",
        //    btn0.state, btn1.state, btn2.state, btn3.state, btn4.state, btn5.state, btn6.state, btn7.state);
        //UART_puts(chr);

        //sprintf (chr, "0: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d \n",
        //    btn0.sum, btn1.sum, btn2.sum, btn3.sum, btn4.sum, btn5.sum, btn6.sum, btn7.sum);
        //UART_puts(chr);
        //UART_puts("\n");
        //_delay_ms(100);
        if (send == 1) {
            // high - enable trancseiver
            PORTD |= (1 << PD2);
            for (i = 0; i < 5; i++) {
                PORTB ^= (1 << PB4);
                UART_putc((char)value);
                _delay_ms(20);
            }
            send = 0;
        }
        // low - enable receiver
        PORTD &= ~(1 << PD2);

        // switch off background light
        if (value == 0) {
            PORTC |= (1 << PC6);
        } else {
            PORTC &= ~(1 << PC6);
        }
    }
}

void pinsInit(void) {
    // all B pins as input
    DDRB = 0x00;
    PORTB = 0x00;
    DDRB |= (1 << PB4);

    DDRC = 0x00;
    PORTC = 0x00;

    DDRC  |= (1 << PC6);

    DDRD = 0x00;
    PORTD = 0x00;

    //DDRD  |= (1 << PD3) | (1 << PD4);
    DDRD  |= (1 << PD2);
}
