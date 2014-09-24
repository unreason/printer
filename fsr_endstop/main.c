/*
 * Project: FSR endstop controller for 3d printer bed leveling.
 * The MCU has FSRs (Interlink 30-81794, 0.5" dia, or similar) connected to its analog pins.
 * One side of the FSR is grounded, the other side is pulled up to Vcc through a 10k resistor.
 *
 * The controller samples and filters the analog signal, and drives a digital output when
 * one of the FSRs are triggered.
 *
 * The endstop is considered to be triggered when the low-pass filtered sample is below 0.92 of
 * the long-term (~2s) average for that FSR. This idea is taken from JohnSL's AVR endstop code
 * but the implementation is slightly different.
 *
 * main.c
 */

#include <msp430.h>
#include "drivers/uart.h"
#include "drivers/led.h"
#include "drivers/adc.h"


#define DELAY_MICROSECONDS(x) __delay_cycles(x*16)




void mcu_init(void) {
	// Turn off WDT
	WDTCTL = WDTPW | WDTHOLD;

	// Set clock to 16MHz
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL = CALDCO_16MHZ;

	BCSCTL2 = SELM_0 | DIVM_0 | DIVS_0;

	// turn off the crystal oscillator

	led_off();
	__bis_SR_register(OSCOFF);

	// wait for DCO to be up and running
	while (IFG1 & OFIFG) {
		IFG1 &= ~OFIFG;
	}
}


#define ADC_PERIOD_IN_TIMER_TICKS 200

void timer_init(void) {
	// set up timer A0 to run at a 2MHz rate, and to trigger a 10khz output (count UP to 200)
	// this is used to trigger ADC at a predictable sample rate.

	TACCR0 = ADC_PERIOD_IN_TIMER_TICKS-1;	// counts from 0 to TACCR inclusively

	TACCTL0 = CCIE; // trigger interrupt when reached max count

	TACTL = TASSEL_2 /* SMCLK source */
			| ID_3 /* divide by 8 */
			| MC_1 /* up mode */
			| TACLR;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER_ISR(void) {
	__low_power_mode_off_on_exit();
}


// for a 2s response IIR filter the decay coefficient should be 2*0.9999.
// to make the computation faster we'll use fixed point arithmetic
// and add the sample with a 1/16384 scaling factor (correspondingly, the
// decay coefficient is 1-1/16384.
// storing the averages as 32-bit values (16b int/16b frac).


long samples[3];
long baseline[3];
long denoised[3];

#define IIR_LOWPASS(avg,sample,bits) (avg) = (avg) - ((avg)>>(bits)) + ((sample)<<(16-(bits)))

// momentary value dropping below ~0.9 of the average constitutes triggering.
#define TRIGGERED(momentary,baseline) (((momentary) + ((momentary) >> 3)) < (baseline))

void main(void) {

	led_init();

	led_on();
	mcu_init();

	timer_init();
	adc_init();
	_enable_interrupts();


	P2DIR |= 0x01;
	P2SEL &= ~0x01;
	P2SEL2 &= 0x01;

	led_off();

	while (1) {
		// wait for the sample clock to trigger
		__low_power_mode_0();

//		led_on();
		samples[0] = adc_read(3);
		samples[1] = adc_read(4);
		samples[2] = adc_read(5);

		IIR_LOWPASS(baseline[0],samples[0],13);
		IIR_LOWPASS(baseline[1],samples[1],13);
		IIR_LOWPASS(baseline[2],samples[2],13);

		IIR_LOWPASS(denoised[0],samples[0],3);
		IIR_LOWPASS(denoised[1],samples[1],3);
		IIR_LOWPASS(denoised[2],samples[2],3);

		if (TRIGGERED(denoised[0], baseline[0]) ||
			TRIGGERED(denoised[1], baseline[1]) ||
			TRIGGERED(denoised[2], baseline[2])) {
			led_on();
			P2OUT |= 0x01;
		} else {
			led_off();
			P2OUT &= ~0x01;
		}
	}
}


