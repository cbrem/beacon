#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <math.h>  //include libm

#include "mpu6050/mpu6050.h"
#include "usart/usart.h"
#include "leds/leds.h"
#include "math3d/math3d.h"

// Number of milliseconds between cycles.
#define CYCLE_LENGTH 100

// If the dot product a normalized gravity vector with one of the glove's
// axes is above this threshold, we will consider them parallel.
#define SIGNAL_THRESHOLD 0.75

// Globals
// TODO: bad to have globals?
double quatW, quatX, quatY, quatZ;
double gravX, gravY, gravZ;

// Configures microcontroller and peripherals. Runs once, at startup.
// Returns 0 if successful, non-zero error otherwise.
uint8_t main_init() {

	// Init I/O LEDS.
	leds_init();

	// Init mpu6050 accel/gyro and integrated DMP chip.
	mpu6050_init();
	_delay_ms(50);
	if (mpu6050_dmpInitialize()) {
		return 1;
	}

	// TODO: taken from jrowberg's c++ code...do we need this?
	mpu6050_setXGyroOffset(220);
    mpu6050_setYGyroOffset(76);
    mpu6050_setZGyroOffset(-85);

	mpu6050_dmpEnable();
	_delay_ms(10);

	// TODO: Init loop timer.

    // Init serial communications.
	usart_init();

	// Enable interrupts.
	sei();

	// Setup was successful.
	return 0;
}

// Updates the global roll, pitch, and yaw variables with information about
// the MPU's orientation.
//
// TODO: add timeout
void get_orientation() {
	if(mpu6050_getQuaternionWait(&quatW, &quatX, &quatY, &quatZ)) {
		mpu6050_getGravity(quatW, quatX, quatY, quatZ, &gravX, &gravY, &gravZ);
	}
}

// Log information about the MPU's orientation over the serial connection.
void log_orientation() {
	char tmp[10];

	usart_puts("gravX: "); dtostrf(gravX, 3, 5, tmp); usart_puts(tmp); usart_putc(' ');
	usart_puts("gravY: "); dtostrf(gravY, 3, 5, tmp); usart_puts(tmp); usart_putc(' ');
	usart_puts("gravZ: "); dtostrf(gravZ, 3, 5, tmp); usart_puts(tmp); usart_putc(' ');

	usart_puts("    ");

	usart_puts("quatW: "); dtostrf(quatW, 3, 5, tmp); usart_puts(tmp); usart_putc(' ');
	usart_puts("quatX: "); dtostrf(quatX, 3, 5, tmp); usart_puts(tmp); usart_putc(' ');
	usart_puts("quatY: "); dtostrf(quatY, 3, 5, tmp); usart_puts(tmp); usart_putc(' ');
	usart_puts("quatZ: "); dtostrf(quatZ, 3, 5, tmp); usart_puts(tmp); usart_putc(' ');

	usart_putc('\n');
}

// Update the indicator LEDS.
void update_leds() {
	if (math3d_dot(gravX, gravY, gravZ, 1.0, 0.0, 0.0) >= SIGNAL_THRESHOLD) {
		// The glove's positive x-axis is pointing roughly upward,
		// so the biker is signalling right.
		leds_set(LEDS_RIGHT);
	} else if (math3d_dot(gravX, gravY, gravZ, 0.0, -1.0, 0.0) >= SIGNAL_THRESHOLD) {
		// The glove's negative y-axis is pointing roughly upward,
		// so the biker is signalling left.
		leds_set(LEDS_LEFT);
	} else {
		// The biker is not signalling.
		leds_set(LEDS_OFF);
	}
}

// Main routine.
int main(void) {

	// Attempt setup, and idle if it fails.
	if (main_init()) {
		for (;;) {}
	}	

	// Main loop.
	usart_puts("Beginning main loop.\n");
	for (;;) {
		_delay_ms(CYCLE_LENGTH);

		get_orientation();
		log_orientation();
		update_leds();
	}

	return 0;
}
