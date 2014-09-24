#ifndef __UART_H__
#define __UART_H__


void uart_init(void);
void uart_tx_byte(char byte);
void uart_tx(const char *data, int count);
void uart_tx_string(const char *data);

char uart_rx_byte();
char uart_rx_byte_blocking();

#endif