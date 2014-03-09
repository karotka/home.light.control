/*
 *
 */

#define F_CPU 8000000UL
#ifndef TOUCH_H_
#define TOUCH_H_
#include <avr/io.h>


/** Holds information related to a single touch channel */
typedef struct {
    volatile uint8_t *port;    //PORTx register for pin
    volatile uint8_t portmask; //mask for the bit in port
    volatile uint8_t mux;      //ADMUX value for the channel
    volatile uint16_t sum;     //measure 16 times
    volatile uint8_t state;    //state of the button
    volatile uint8_t lstate;   //last state of the button
    volatile uint8_t count;    //measure counter
    volatile uint16_t value;   //measure value
} touch_channel_t;

enum {btnOff = 0, btnOn, btnRelease};

//////////////////////////////////////////////////////////////////////////
//
// Library functions
//
//////////////////////////////////////////////////////////////////////////

// Library initialization
// Call this once to initialize the library functions and the ADC converter
void TOUCH_init(void);
void TOUCH_down(void);

// The sense function evaluates the button state and performs internal
inline void adc_channel(uint8_t channel);

inline uint16_t adc_get(void);

uint16_t touch_measure(touch_channel_t *btn);
uint16_t touch_measure1(touch_channel_t *btn);

void btn_set(touch_channel_t *btn);
#endif
