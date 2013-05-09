/*
 *  fancontrol_attiny - fan speed control using Atmel ATTINY85
 *
 *  Written in 2013 by Victor Antonovich (v.antonovich@gmail.com)
 *
 *  To the extent possible under law, the author(s) have dedicated
 *  all copyright and related and neighboring rights to this software
 *  to the public domain worldwide. This software is distributed without
 *  any warranty.
 *  You should have received a copy of the CC0 Public Domain Dedication
 *  along with this software. If not, see
 *  http://creativecommons.org/publicdomain/zero/1.0/.
 */

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

/* Maximum PWM duty cycle */
#define PWM_MAX 255
/* Minimum PWM duty cycle (non-zero to prevent motor stall) */
#define PWM_MIN 60

/* Difference between motor start and stop temperatures, celsius */
#define MOTOR_HYST_TEMP 5
/* Motor start temperature, celsius */
#define MOTOR_START_TEMP 40
/* Motor stop temperature, celsius. Lower than start temperature to add some hysteresis */
#define MOTOR_STOP_TEMP (MOTOR_START_TEMP - MOTOR_HYST_TEMP)
/* Motor full speed temperature, celsius. */
#define MOTOR_FULL_TEMP 80

/* ADC threshold for disconnected NTC thermistor. */
#define ADC_THR_DISC 16
/* ADC base threshold. */
#define ADC_THR_BASE 512

#define MOTOR_PWM(temp) PWM_MIN + ((uint16_t) temp - MOTOR_STOP_TEMP) * \
	(PWM_MAX - PWM_MIN) / (MOTOR_FULL_TEMP - MOTOR_STOP_TEMP);

enum { MOTOR_OFF, MOTOR_ON };

/* ADC shifted 8-bit value to temperature (celsius) lookup table */
const uint8_t TEMPS[256] PROGMEM = { 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27,
		27, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30,
		31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 34, 34,
		34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37,
		38, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41,
		41, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45,
		46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50,
		50, 50, 51, 51, 51, 51, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55,
		55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 60, 60, 60,
		61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 66, 66, 66, 67, 67,
		68, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 73, 74, 74, 75, 76,
		76, 77, 77, 78, 78, 79, 79, 80, 81, 81, 82, 82, 83, 84, 84, 85, 86, 87,
		87, 88, 89, 90, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102,
		103, 105, 106, 107, 109, 110, 112, 113, 115, 117, 119, 121, 123, 125,
		128, 130, 133, 136, 139, 143, 147, 152, 157, 163, 170, 179, 190, 205,
		228, 255 };

volatile uint8_t motor_state = MOTOR_OFF;

ISR(ADC_vect) {
	uint8_t temp, motor_pwm;
	uint16_t adc = ADC;
	if (adc < ADC_THR_DISC) {
		/* Looks like NTC thermistor is disconnected or broken,
		 * set motor at full speed as failsafe action */
		temp = MOTOR_FULL_TEMP;
	} else if (adc < ADC_THR_BASE) {
		/* Temperature is below 25C */
		temp = MOTOR_STOP_TEMP;
	} else {
		/* Get actual temperature using lookup table */
		temp = pgm_read_byte(&TEMPS[(adc - ADC_THR_BASE) >> 1]);
		if (temp > MOTOR_FULL_TEMP) {
			temp = MOTOR_FULL_TEMP;
		}
	}
	/* Set motor speed according to measured temperature */
	if (motor_state == MOTOR_ON) {
		if (temp < MOTOR_STOP_TEMP) {
			motor_pwm = 0;
			motor_state = MOTOR_OFF;
		} else {
			motor_pwm = MOTOR_PWM(temp);
		}
	} else {
		if (temp > MOTOR_START_TEMP) {
			motor_pwm = MOTOR_PWM(temp);
			motor_state = MOTOR_ON;
		} else {
			motor_pwm = 0;
		}
	}
	/* Set PWM output */
	OCR1B = motor_pwm;
	/* Start next A/D conversion */
	ADCSRA |= _BV(ADSC);
}

void ioinit(void) {
	/* Enable fast peripheral clock PLL in low speed (32 MHz) mode */
	PLLCSR |= _BV(PLLE) | _BV(LSM);
	/* Waiting PLL lock */
	while(!(PLLCSR & _BV(PLOCK)));
	/* Enable fast peripheral clock as Timer 1 clock source */
	PLLCSR |= _BV(PCKE);

	/* Start Timer 1 clocked at PCK */
	TCCR1 = _BV(CS10);
	/* Enable PWM mode on OC1B */
	GTCCR = _BV(PWM1B) | _BV(COM1B1);
	/* Set Timer 1 TOP value */
	OCR1C = 255;
	/* Set PWM value to 0 */
	OCR1B = 0;
	/* Enable OC1B as output, other pins are inputs */
	DDRB = _BV(DDB4);

	/* Setup ADC: enable ADC, autotrigger mode, enable interrupt, prescaler CLK/128 */
	ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	/* Set ADC multiplexer: Vcc as reference, channel 3 (pin PB3) */
	ADMUX = _BV(MUX1) | _BV(MUX0);

	/* Enable interrupts */
	sei();

	/* Start A/D conversion */
	ADCSRA |= _BV(ADSC);
}

int main(void) {
	ioinit();

	/* Loop forever, all work is done in the interrupts */
	for (;;) {
		sleep_mode();
	}

	return (0);
}
