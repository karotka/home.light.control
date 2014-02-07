/*
 *
 */

#define F_CPU 8000000UL
#ifndef TOUCH_H_
#define TOUCH_H_
#include <avr/io.h>


/** Holds information related to a single touch channel */
typedef struct {
    volatile uint8_t *port; //PORTx register for pin
    volatile uint8_t portmask; //mask for the bit in port
    volatile uint8_t mux; //ADMUX value for the channel
} touch_channel_t;

enum {tt_off=0,tt_on,tt_push,tt_release,tt_timeout};

//////////////////////////////////////////////////////////////////////////
//
// Library functions
//
//////////////////////////////////////////////////////////////////////////

// Library initialization
// Call this once to initialize the library functions and the ADC converter
void TOUCH_init(void);

// The sense function evaluates the button state and performs internal
// housekeeping. It should be polled at least 30 times per second to
// update the internal logic. Please note that each call performs 32
// analog to digital conversions with active waiting. This may take
// several ms.

// Possible return values are:
//		tt_off=0	No touch sensed
//		tt_on		Touch button is active and touch is sensed.
//		tt_push		Touch button is pushed. Use this to initiate one time events.
//		tt_release	Touch button is released. Use this to initiate one time events.
//		tt_timeout	Touch button has been active too long and internal bias was	reset.
uint8_t TOUCH_sense(touch_channel_t *channel);

inline void adc_channel(uint8_t channel);

inline uint16_t adc_get(void);

uint16_t TOUCH_measure(touch_channel_t *channel);

#endif
