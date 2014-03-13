#include <avr/io.h>

// States that the leds can indicate.
typedef enum {
    LEDS_OFF,
    LEDS_LEFT,
    LEDS_RIGHT
} leds_t;

void leds_init();
void leds_set(leds_t setting);
