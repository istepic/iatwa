#ifndef UART_H
#define UART_H

#include "nrf_uart.h"
#include "app_uart.h"

uint32_t uart_init(void);

#define UART_RX_PIN 27
#define UART_TX_PIN 26

#define UART_TX_BUF_SIZE 256
#define UART_RX_BUF_SIZE 256

#endif
