#define F_CPU 8000000UL
#include <util/delay.h>
#include "touch.h"

/*
 * Capacitive sensing using charge sharing between
 * the S/H capacitor and an external sensing pad
 * Internal function to read the adc input
 */

/**
 */
void TOUCH_init(void) {
    ADMUX |= (1 << REFS0);                  // reference AVCC (5v)
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1);  // clockiv 64
    ADCSRA |= (1 << ADEN) | (0 << ADIE);    // enable ADC 8MHz/64 = 125kHz
}

inline void adc_channel(uint8_t channel) {
    ADMUX = (ADMUX & 0xf0) | (channel & 0x0f);
}

inline uint16_t adc_get(void) {
    ADCSRA |= (1 << ADSC);                  //start conversion
    //while(!(ADCSRA & (1 << ADIF)));         //wait for conversion to finish
    while(ADCSRA & (1 << ADSC));         //wait for conversion to finish
    ADCSRA |= (1 << ADIF);                  //reset the flag
    return ADC;                             //return value
}


uint16_t touch_measure(touch_channel_t *btn) {
    uint8_t i;
    uint16_t adc;

    adc = 0;
    // Do four measurements and average, just to smooth things out
    for (i = 0 ; i < 4; i++){
        DDRC  |= (btn->portmask);
        PORTC |= (btn->portmask);       //set pullup on

        _delay_us(10);

        DDRC &= ~(btn->portmask);
        PORTC &= ~(btn->portmask);      //set pullup off

        _delay_us(10);

        adc_channel(0b11111);           //set ADC mux to ground;
        adc_get();                      //do a measurement (to discharge the sampling cap)
        adc_channel(btn->mux);          //set ADC mux to ground;
        adc += adc_get();               //do a measurement (to discharge the sampling cap)
    }
    return adc / 4;
}

void btn_set(touch_channel_t *btn) {
    btn->value = touch_measure(btn);
    btn->sum  += btn->value;
    btn->count++;

    // reset the counter
    if (btn->count == 4) {

        if (btn->sum / 4 > 50) {
            btn->state = btnOn;
        } else {
            btn->state = btnOff;
        }

        if (btn->lstate == btnOn && btn->state == btnOff) {
            btn->state = btnRelease;
        }

        btn->lstate = btn->state;
        btn->count = 0;
        btn->sum = 0;
    }
}
