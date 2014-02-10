#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
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
//
//void pwmInit(void);
//void watchDogInit(void);
void pinsInit(void);
void pinsInit(void);
void timer0Init(void);
//void readAddress(volatile uint8_t *port, volatile uint8_t *addr);


volatile uint8_t btn1State = 0;

ISR (TIMER0_OVF_vect) { // timer0 overflow interrupt
    cli();

    btn_set(&btn1);
    btn_set(&btn2);
    btn_set(&btn3);
    btn_set(&btn4);

    if (btn1.state == btnRelease) {
        btn1.state = btnOff;
        UART_puts("Button1");
        UART_putc('\n');
    }
    if (btn2.state == btnRelease) {
        btn2.state = btnOff;
        UART_puts("Button2");
        UART_putc('\n');
    }
    if (btn3.state == btnRelease) {
        btn3.state = btnOff;
        UART_puts("Button3");
        UART_putc('\n');
    }
    if (btn4.state == btnRelease) {
        btn4.state = btnOff;
        UART_puts("Button4");
        UART_putc('\n');
    }
    sei();
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
    //watchDogInit();
    //pwmInit();
    timer0Init();

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

    //uint16_t sample1, sample2, sample3, sample4;
    //uint8_t temp, value;

    for(;;) {

        //value = 0;
        //if (!TWI_TransceiverBusy()) {
        //    //PORTD |= (1 << PD7);
        //    if (TWI_statusReg.RxDataInBuf) {
        //        TWI_GetDataFromTransceiver(&temp, 1);
        //    }
        //    TWI_StartTransceiverWithData(&temp, 1);
        //    UART_puti((uint8_t)temp);
        //    //UART_putc(temp);
        //    UART_putc('\n');
        //    value = temp;
        //}

        //// Button 1
        //if (btn1State == btnRelease) {
        //    btn1State = btnOff;
        //    UART_puti(btnRelease);
        //    UART_putc('\n');
        //}
        //sample1 = touch_measure(&btn1);
        //if (sample1 > 50) {
        //    UART_puti(sample1);
        //    UART_putc('\n');
        //    //TWI_StartTransceiverWithData(&value, 1);
        //}

        //// Button 2
        //sample2 = TOUCH_measure(&btn2);
        //if (sample2 > 80) {
        //    value = 2;
        //    UART_puti(value);
        //    UART_putc('\n');
        //    //TWI_StartTransceiverWithData(&value, 1);
        //}
        //
        //// Button 3
        //sample3 = TOUCH_measure(&btn3);
        //if (sample3 > 80) {
        //    value = 3;
        //    UART_puti(value);
        //    UART_putc('\n');
        //    //TWI_StartTransceiverWithData(&value, 1);
        //}
        //
        //// Button 3
        //sample4 = TOUCH_measure(&btn4);
        //if (sample4 > 80) {
        //    value = 4;
        //    UART_puti(value);
        //    UART_putc('\n');
        //    //TWI_StartTransceiverWithData(&value, 1);
        //}

        _delay_ms(100);
    }
}

//void pwmInit(void) {
//    TCCR0A = (1 << COM0A1) | (1 << WGM00);
//    TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A1) | (0 << COM0A0);
//    TCCR0B = (1 << CS01);
//}



void watchDogInit(void) {
    // Watch dog every 0.25s and power down
    WDTCSR |= (1 << WDP2);// | (1 << WDP2);
    WDTCSR |= (1 << WDIE);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void pinsInit(void) {
    DDRD = 0x00;
    PORTD = 0x00;
    //DDRD |= (1 << PD5) | (1 << PD6) | (1 << PD7);

    DDRC = 0x00;
    PORTC = 0x00;
    //DDRC |= (1 << PD0) | (1 << PD7);// | (1 << PD7);

    // all B pins as input
    DDRB = 0x00;
    PORTB = 0x00;
    //DDRB |= (1 << PB7);
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
