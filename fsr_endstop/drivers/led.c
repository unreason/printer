#include "led.h"

#include <msp430.h>

#define LED_REG_PORTOUT P1OUT
#define LED_REG_PORTDIR P1DIR

#define LED_BIT BIT0

void led_init(void)
{
	LED_REG_PORTDIR |= LED_BIT;
}

void led_on(void)
{
	LED_REG_PORTOUT |= LED_BIT;
}

void led_off(void)
{
	LED_REG_PORTOUT &= ~LED_BIT;
}

void led_toggle(void)
{
	LED_REG_PORTOUT ^= LED_BIT;
}
