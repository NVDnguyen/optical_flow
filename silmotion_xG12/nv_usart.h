/*
 * usart.h
 *
 *  Created on: May 18, 2025
 *      Author: nvd
 */

#ifndef USART_H
#define USART_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*usart_rx_callback_t)(uint8_t data);
typedef void (*usart_tx_callback_t)(void);

// USART initialization
void usart_init(void);

// Register RX callback
void usart_set_rx_callback(usart_rx_callback_t cb);

// Register TX callback
void usart_set_tx_callback(usart_tx_callback_t cb);

// Transmit a byte (non-blocking)
void usart_send(uint8_t data);

int usart_printf(const char *format, ...);
#endif // USART_H
