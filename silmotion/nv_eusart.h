/*
 * eusart.h
 *
 *  Created on: May 18, 2025
 *      Author: nvd
 */

#ifndef EUSART_H
#define EUSART_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*eusart_rx_callback_t)(uint8_t data);
typedef void (*eusart_tx_callback_t)(void);

// EUSART initialization
void eusart_init(void);

// Register RX callback
void eusart_set_rx_callback(eusart_rx_callback_t cb);

// Register TX callback
void eusart_set_tx_callback(eusart_tx_callback_t cb);

// Transmit a byte (non-blocking)
void eusart_send(uint8_t data);

#endif // EUSART_H
