/*
 *
 * Created: 09.06.2013 17:25:47			-	v0.1 Initial release (Attiny 10)
 *			10.06.2013					- 	v0.2 ported to ATtiny 25/45/85 and ATtiny13
 *  Author: Tim (cpldcpu@gmail.com)
 */

#define F_CPU 8000000UL
#ifndef TOUCH_H_
#define TOUCH_H_
#include <avr/io.h>


/** Holds information related to a single touch channel */
typedef struct {
    volatile uint8_t portmask; //mask for the bit in port
    volatile uint8_t mux; //ADMUX value for the channel
} touch_channel_t;


enum {tt_off=0,tt_on,tt_push,tt_release,tt_timeout};

//////////////////////////////////////////////////////////////////////////
//
// User definable settings
//
//////////////////////////////////////////////////////////////////////////

// Define upper and lower threshold for the touch sensing. You may have to
// change these depending on the geometry of your touch button.
// Setting the "on" value lower will make the touch button more sensitive.
// Setting the "off" value higher will make the touch button less likely
// to be "stuck". Too high values can lead to oscillations.

#define touch_threshold_on 200
#define touch_threshold_off 10

// If the touch button is pressed, the bias value is not updated
// to prevent detection of multiple touches. In some cases this may
// lead to a "stuck button" situation. Therefore an update of the bias
// is forced after a certain time unless this function is deactivated.
//
// The value of timeout corresponds to the number of calls to tinytouch_sense().
// The maximum is 255.
// Setting timeout to 0 will turn the functionality off.

//#define touch_timeout 255
#define touch_timeout 0		// turn off timeout functionality


// Define pins to use for the reference input and the touch button
// The reference pin is used to charge or discharge the internal
// sample&hold capacitor. This pin is used in output mode and should
// not be shorted to VCC or GND externally.
// The sense pin is connected to the touch-button. To improve noise immunity
// a series resistor can be used.

// The pin number must match the corresponding analog input number ADCx.
// Default port is PORTB. (ATtiny 5/10/13/25/45/85)

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

uint16_t touch_adc(touch_channel_t *channel);

#endif
