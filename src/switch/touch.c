#define F_CPU 8000000UL
#include <util/delay.h>
#include "touch.h"

// Internal function to read the adc input

uint16_t bias;
uint8_t	touch;
#if touch_timeout>0
uint8_t timer;
#endif
/*
			Capacitive sensing using charge sharing between
			the S/H capacitor and an external sensing pad
*/

void TOUCH_init(void) {
    ADMUX |= (1 << REFS0); //reference AVCC (5v)
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1); //clockiv 64
    //final clock 8MHz/64 = 125kHz
    ADCSRA |= (1 << ADEN); //enable ADC
}

inline void adc_channel(uint8_t channel) {
    ADMUX = (ADMUX & 0xf0) | (channel & 0x0f);
}

uint8_t TOUCH_sense(touch_channel_t *channel) {
	uint8_t i;
	uint16_t tmp;
	int16_t delta;

	tmp=0;
	for (i=0; i<16; i++) {
		//tmp += touch_adc();	// average 16 samples
		_delay_us(100);
	}

	delta=tmp-(bias>>4);

	if (!touch)	{
		if (delta>touch_threshold_on) {
			touch=1;
#if touch_timeout>0
			timer=0;
#endif
			return tt_push;
		}

		// update bias only when touch not active
		bias=(bias-(bias>>6))+(tmp>>2);		// IIR low pass
		return tt_off;
	} else {
		if (delta<touch_threshold_off) {
			touch=0;
			return tt_release;
		}

#if touch_timeout>0
		if (timer==255) {
			bias = touch_adc()<<8;
			return tt_timeout;
		}
		timer++;
#endif
		return tt_on;
	}
}

inline uint16_t adc_get(void) {
    ADCSRA |= (1 << ADSC); //start conversion
    while(!(ADCSRA & (1 << ADIF))); //wait for conversion to finish
    ADCSRA |= (1 << ADIF); //reset the flag
    return ADC; //return value
}

uint16_t touch_adc(touch_channel_t *channel) {
    uint8_t i;
	uint16_t adc = 0;
    for (i = 0; i < 8; i++) {
	    //DDRC  |= (channel->portmask);
        //PORTC |= (channel->portmask); // set pullup on
        //
	    //_delay_us(32);
        //
	    //DDRC &= ~(channel->portmask);
        //PORTC &= ~(channel->portmask); // set pullup off
        //
	    //_delay_us(32);
        //
        //adc_channel(channel->mux); //set ADC mux to ground;
        //adc += adc_get(); //do a measurement (to discharge the sampling cap)


	    DDRC  |= (1 << PC3);
        PORTC |= (1 << PC3); // set pullup on

	    _delay_us(32);

	    DDRC &= ~(1 << PC3);
        PORTC &= ~(1 << PC3); // set pullup off

	    _delay_us(32);

        adc_channel(3); //set ADC mux to ground;
        adc += adc_get(); //do a measurement (to discharge the sampling cap)

	    _delay_us(100);
    }
	return adc / 8;
}
