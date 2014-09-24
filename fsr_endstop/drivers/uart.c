#include "uart.h"
#include <msp430.h>


#ifndef __MSP430_HAS_USCI__
#error "This driver only supports USCI"
#endif

#define USCI_REG_CTL0   UCA0CTL0
#define USCI_REG_CTL1   UCA0CTL1
#define USCI_REG_BR0    UCA0BR0
#define USCI_REG_BR1    UCA0BR1
#define USCI_REG_TXBUF  UCA0TXBUF
#define USCI_REG_RXBUF  UCA0RXBUF
#define USCI_REG_STAT	UCA0STAT
#define USCI_REG_MCTL	UCA0MCTL

#define USCI_REG_IFG	IFG2

#define USCI_BIT_TXIFG	UCA0TXIFG
#define USCI_BIT_RXIFG	UCA0RXIFG


#define UART_PORT_OUT     P1OUT
#define UART_PORT_REG_DIR P1DIR
#define UART_PORT_REG_SEL P1SEL
#define UART_PORT_REG_SEL2 P1SEL2

#define UART_PIN_TX		BIT2
#define UART_PIN_RX		BIT1

void uart_init(void) 
{
	// Per the datasheet (SLAU144I), USCI has to be in reset while configuration is changed

	// Put USCI in reset
	USCI_REG_CTL1 |= UCSWRST;

	// Configure the I/O pins
	UART_PORT_REG_DIR |= UART_PIN_TX;
	UART_PORT_REG_DIR &= ~UART_PIN_RX;

	UART_PORT_REG_SEL |= UART_PIN_RX | UART_PIN_TX;
	UART_PORT_REG_SEL2 |= UART_PIN_RX | UART_PIN_TX;

	// Perform configuration
	// Configure SPI as LSB first (UCMSB not set), UART mode (UCMODE_0)
	USCI_REG_CTL0 = UCMST | UCMODE_0;

	// Clock source is SMCLK (UCSSEL_3), still off (UCSWRST)
	USCI_REG_CTL1 = UCSSEL_3 | UCSWRST;

	// Set the baud rate as 9600 (the only rate supported by the launchpad USB->serial converter)
	USCI_REG_BR0 = 104; USCI_REG_BR1 = 0;
	USCI_REG_MCTL = UCBRS_1;

#ifdef UART_LOOPBACK
	// Make the SPI interface loopback for debugging
	USCI_REG_STAT &= ~UCLISTEN;

#endif

	// Release USCI from reset
	USCI_REG_CTL1 &= ~UCSWRST;
}


void uart_tx_byte(char byte)
{
	USCI_REG_TXBUF = byte;
	while (USCI_REG_STAT & UCBUSY);
}

void uart_tx(const char *data, int count) 
{
	int i = 0;
	for (i = 0; i < count; i++) 
	{
		uart_tx_byte(*(data + i));
	}
}

void uart_tx_string(const char *data)
{
	while (*data) 
	{
		uart_tx_byte(*data);
		data++;
	}
}

char uart_rx_byte()
{
	return USCI_REG_RXBUF;
}

char uart_rx_byte_blocking()
{
	while (!(USCI_REG_IFG & USCI_BIT_RXIFG));
	return uart_rx_byte();
}
