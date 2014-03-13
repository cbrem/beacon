#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <math.h>  //include libm

#include "mpu6050/mpu6050.h"
#include "usart/usart.h"
#include "leds/leds.h"

// Number of milliseconds between cycles.
#define CYCLE_LENGTH 10000

// Constants for signal boundaries.
// TODO: experiment to find these
#define SIGNAL_ROLL_LOW 0
#define SIGNAL_ROLL_HIGH 1
#define LEFT_YAW_LOW 0
#define LEFT_YAW_HIGH 1
#define RIGHT_YAW_LOW 1
#define RIGHT_YAW_HIGH 2

// Globals
// TODO: is this good practice in embedded programming to declare globals
// like this in order to reduce memory usage?
double qw = 1.0f;
double qx = 0.0f;
double qy = 0.0f;
double qz = 0.0f;
double roll = 0.0f;
double pitch = 0.0f;
double yaw = 0.0f;

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
	if(mpu6050_getQuaternionWait(&qw, &qx, &qy, &qz)) {
		mpu6050_getRollPitchYaw(qw, qx, qy, qz, &roll, &pitch, &yaw);
	}
}

// Log information about the MPU's orientation over the serial connection.
void log_orientation() {
	char tmp[10];
	usart_puts("ROLL: "); dtostrf(roll, 3, 5, tmp); usart_puts(tmp); usart_putc(' ');
	usart_puts("PITCH: "); dtostrf(pitch, 3, 5, tmp); usart_puts(tmp); usart_putc(' ');
	usart_puts("YAW: "); dtostrf(yaw, 3, 5, tmp); usart_puts(tmp); usart_putc(' ');
	usart_putc('\n');
}

// Update the indicator LEDS.
void update_leds() {
	if (SIGNAL_ROLL_LOW <= roll && roll < SIGNAL_ROLL_HIGH) {
		// The glove's roll (x-axis) indicates that the rider may be signalling.
		if (LEFT_YAW_LOW <= yaw && yaw < LEFT_YAW_HIGH) {
			// The glove's yaw indicates that the rider is signalling left.
			leds_set(LEDS_LEFT);
		} else if (RIGHT_YAW_LOW <= yaw && yaw < RIGHT_YAW_HIGH) {
			// The glove's yaw indicates that the rider is signalling right.
			leds_set(LEDS_RIGHT);
		} else {
			// The rider is not signalling.
			leds_set(LEDS_OFF);
		}
	} else {
		// The rider is not signalling.
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
