#include "adc.h"
#include <msp430.h>

#define ADC_PORT_DIR 	P1DIR

#define ADC_CHANNEL_AE		ADC10AE0

// active ADC channels: 3,4,5
#define ADC_ACTIVE_CHANNEL_MASK 0x38


void adc_init(void)
{
	// Configure the pins as inputs
	ADC_PORT_DIR &= ~ADC_ACTIVE_CHANNEL_MASK;

	// Switch the analog input on the pin ON
	ADC_CHANNEL_AE |= ADC_ACTIVE_CHANNEL_MASK;

	// Power on the ADC peripheral 
	ADC10CTL0 = ADC10ON;

	// Configure the ADC: VCC positive reference, ground negative reference (SREF_0),
	// minimum (4 clocks) S&H time (ADC10SHT_3)
	// reference output on pin off,
	// ADC still on (ADC10ON)
	ADC10CTL0 = SREF_0 | ADC10SHT_0 | ADC10ON;

}

int adc_read(unsigned char channel)
{

	// Select input channel, s&h starts when conversion triggered, clock source is SMCLK
	// (ADC10SSEL_3)
	ADC10CTL1 = (channel << 12) | ADC10SSEL_3;

	// Set the Enable Conversion flag, and start conversion.
	// This can be done with one instruction according to the datasheet.
	ADC10CTL0 |= ENC | ADC10SC;

	// wait until conversion is done
	while (ADC10CTL1 & ADC10BUSY);

	// disable the conversion trigger again
	ADC10CTL0 &= ~ENC;

	return ADC10MEM;
}
