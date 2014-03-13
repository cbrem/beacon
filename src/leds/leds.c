#include "leds.h"

// Configure PB2 and PB1 to hold LEDS.
void leds_init() {
    DDRB |= _BV(2) | _BV(1);
}

void leds_set(leds_t setting) {
    uint8_t mask;
    switch (setting) {
        case LEDS_OFF:
            mask = 0;
            break;
        case LEDS_LEFT:
            mask = _BV(1);
            break;
        case LEDS_RIGHT:
            mask = _BV(2);
            break;
    }
    PORTB &= ~(_BV(2) | _BV(1));
    PORTB |= mask;
}