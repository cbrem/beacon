// TODO: get power-off worked out
//
// Desired behavior:
//   * Press button to turn on (wake from sleep mode)
//   * Press button to turn off (initiate sleep mode)
//   * Enter sleep mode after inactivity
//
// Concerns:
//   * hardware debounce button to prevent multiple on/offs
//   * software debounce button: if an interrupt occurs when
//     we're in the middle of a start-up shut-down, it should
//     have no effect
//   * detect inactivity by having a counter and resetting
//     it whenever the accelerometer moves? we might need to
//     turn down sensitivity for this to actually work.
//   * indicate "powered up and running" to user with LED
//   * check whether power consumption in sleep mode is actually
//     comparable to battery leakage
//   * if AVR sleeps, should MPU sleep too? Or could we just
//     electronically disconnect it?

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

// Number of milliseconds in a flash on/off cycle for the LEDs.
#define FLASH_LENGTH 1000

// Portion of flash cycles for which LEDs can be on.
#define FLASH_DUTY 0.5

// If the dot product a normalized gravity vector with one another vector
// is above this threshold, we will consider them parallel.
#define SIGNAL_THRESHOLD 0.75

// Unit vectors which we will test for being parallel to the gravity vector.
const double rightVect [] = {-0.5 * M_SQRT2, -0.5 * M_SQRT2, 0.0};
const double leftVect [] = {-0.5 * M_SQRT2, 0.5 * M_SQRT2, 0.0};

// Globals
// TODO: bad to have globals?
double quatW, quatX, quatY, quatZ;
double gravX, gravY, gravZ;
uint16_t flashCycleMillis;

// Configures microcontroller and peripherals. Runs once, at startup.
// Returns 0 if successful, non-zero error otherwise.
uint8_t main_init() {

	// Init I/O LEDS, and reset the flash cycle counter.
	flashCycleMillis = 0;
	leds_init();

	// Init mpu6050 accel/gyro and integrated DMP chip.
	mpu6050_init();
	_delay_ms(50);
	if (mpu6050_dmpInitialize()) {
		return 1;
	}
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
	// Update our (rough) count of the number of milliseconds since the
	// beginning of the last flash cycle.
	flashCycleMillis = (flashCycleMillis + CYCLE_LENGTH) % FLASH_LENGTH;

	// If we are not in the ON portion of the flash duty cycle, make sure that
	// the LEDS are off.
	if (flashCycleMillis > FLASH_LENGTH * FLASH_DUTY) {
		leds_set(LEDS_OFF);
		return;
	}

	// Otherwise, set LEDs if we are in a turning position.
	if (math3d_dot(gravX, gravY, gravZ, rightVect[0], rightVect[1], rightVect[2]) >= SIGNAL_THRESHOLD) {
		// The the biker is signalling right.
		leds_set(LEDS_RIGHT);
	} else if (math3d_dot(gravX, gravY, gravZ, leftVect[0], leftVect[1], leftVect[2]) >= SIGNAL_THRESHOLD) {
		// The biker is signalling left.
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
	//usart_puts("Beginning main loop.\n");
	for (;;) {
		_delay_ms(CYCLE_LENGTH);

		get_orientation();
		//log_orientation();
		update_leds();
	}

	return 0;
}
