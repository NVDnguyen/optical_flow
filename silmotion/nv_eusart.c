/*
 * eusart.c
 *
 *  Created on: May 18, 2025
 *      Author: nvd
 */
#include <nv_eusart.h>
#include <stdio.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_eusart.h"
#include "bsp.h"

#define BUFFER_SIZE 4096 //4KB

static uint8_t rx_buffer[BUFFER_SIZE];
static volatile size_t rx_write_index = 0;
static volatile size_t rx_read_index = 0;

static eusart_rx_callback_t rx_callback = 0;
static eusart_tx_callback_t tx_callback = 0;

void eusart_init(void)
{
    // Enable clocks
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_EUSART1, true);

    // Configure GPIO pins
    GPIO_PinModeSet(BSP_BCC_TXPORT, BSP_BCC_TXPIN, gpioModePushPull, 1);
    GPIO_PinModeSet(BSP_BCC_RXPORT, BSP_BCC_RXPIN, gpioModeInput, 0);
    GPIO_PinModeSet(BSP_BCC_ENABLE_PORT, BSP_BCC_ENABLE_PIN, gpioModePushPull, 1);

    // Route EUSART1 pins
    GPIO->EUSARTROUTE[1].TXROUTE = (BSP_BCC_TXPORT << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
                                 | (BSP_BCC_TXPIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
    GPIO->EUSARTROUTE[1].RXROUTE = (BSP_BCC_RXPORT << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
                                 | (BSP_BCC_RXPIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT);
    GPIO->EUSARTROUTE[1].ROUTEEN = GPIO_EUSART_ROUTEEN_RXPEN | GPIO_EUSART_ROUTEEN_TXPEN;

    // EUSART1 default async init (115200, 8N1, no flow control)
    EUSART_UartInit_TypeDef init = EUSART_UART_INIT_DEFAULT_HF;
    EUSART_UartInitHf(EUSART1, &init);

    // Enable interrupts
    NVIC_ClearPendingIRQ(EUSART1_RX_IRQn);
    NVIC_EnableIRQ(EUSART1_RX_IRQn);
    NVIC_ClearPendingIRQ(EUSART1_TX_IRQn);
    NVIC_EnableIRQ(EUSART1_TX_IRQn);

    // Enable RX FIFO level interrupt
    EUSART_IntEnable(EUSART1, EUSART_IEN_RXFL);
}

void eusart_set_rx_callback(eusart_rx_callback_t cb)
{
    rx_callback = cb;
}

void eusart_set_tx_callback(eusart_tx_callback_t cb)
{
    tx_callback = cb;
}

void eusart_send(uint8_t data)
{
    EUSART1->TXDATA = data;
    // Enable TX FIFO level interrupt
    EUSART_IntEnable(EUSART1, EUSART_IEN_TXFL);
}

// RX interrupt handler
void EUSART1_RX_IRQHandler(void)
{
    uint8_t data = (uint8_t)EUSART1->RXDATA;

    // Write data to buffer
    size_t next_index = (rx_write_index + 1) % BUFFER_SIZE;
    if (next_index != rx_read_index) { // Check for buffer overflow
        rx_buffer[rx_write_index] = data;
        rx_write_index = next_index;
    }// else buffer overflow

    EUSART_IntClear(EUSART1, EUSART_IF_RXFL);
}

// TX interrupt handler
void EUSART1_TX_IRQHandler(void)
{
    if (tx_callback) {
        tx_callback();
    } else {
        // If no callback, disable TX interrupt
        EUSART_IntDisable(EUSART1, EUSART_IEN_TXFL);
    }
    EUSART_IntClear(EUSART1, EUSART_IF_TXFL);
}

int eusart_read(uint8_t *data)
{
    if (rx_read_index == rx_write_index) {
        return 0; // Buffer empty
    }

    *data = rx_buffer[rx_read_index];
    rx_read_index = (rx_read_index + 1) % BUFFER_SIZE;
    return 1; //successfully
}
