#include <avr/io.h>

/** Holds information related to a single touch channel */
typedef struct {
    volatile uint8_t *port; //PORTx register for pin
    volatile uint8_t portmask; //mask for the bit in port
    volatile uint8_t mux; //ADMUX value for the channel
} touch_channel_t;

void TOUCH_init(void);

inline void adc_channel(uint8_t channel);

inline uint16_t adc_get(void);

uint16_t TOUCH_measure(touch_channel_t *channel);
