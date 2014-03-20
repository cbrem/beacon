#include "leds.h"

// Configure PB1 - PB4 to hold LEDS.
void leds_init() {
    DDRB |= _BV(4) | _BV(3) | _BV(2) | _BV(1);
}

// Set some combination of LEDs on.
// Note that the LED connected to PB1 is active-high,
// while the LED banks connected to PB2 - PB4 are active-low.
void leds_set(leds_t setting) {
    uint8_t mask = 0;
    switch (setting) {

        case LEDS_OFF:
            // Turn the PB1 LED on.
            mask |= _BV(1);

            // Turn banks PB2 - PB4 off.
            mask |= _BV(4) | _BV(3) | _BV(2);
            break;

        case LEDS_LEFT:
            // Turn the PB1 LED on.
            mask |= _BV(1);

            // Turn off bank PB2, leave banks PB3 and PB4 on.
            mask |= _BV(2);
            break;

        case LEDS_RIGHT:
            // Turn the PB1 LED on.
            mask |= _BV(1);

            // Turn off bank PB4, leave banks PB2 and PB3 on.
            mask |= _BV(4);
            break;
    }

    // Change the output value for PB1 - PB4 according to the mask values.
    PORTB &= ~(_BV(4) | _BV(3) | _BV(2) | _BV(1));
    PORTB |= mask;
}