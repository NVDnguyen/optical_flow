/*
 * usart.c
 *
 *  Created on: May 18, 2025
 *      Author: nvd
 *
 * This file implements the USART interface for asynchronous communication on
 * Silicon Labs EFR32MG12 devices (BRD4161A with EFR32MG12P432F1024GL125).
 * It configures USART2 for serial communication with TX on PA6 and RX on PA7,
 * handles RX and TX interrupts, and supports callback functions for data
 * reception and transmission completion.
 *
 * Based on Silicon Labs example for xG12 (main_xg1_xg12_xg13_xg14.c).
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "nv_usart.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "bsp.h"
#define USART_TX_BUFFER_SIZE 128

// Callback function pointers
static usart_rx_callback_t rx_callback = NULL;
static usart_tx_callback_t tx_callback = NULL;

/*
 * Initialize GPIO for VCOM communication
 */
static void init_gpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure VCOM transmit pin as output
  GPIO_PinModeSet(BSP_VCOM_TXPORT, BSP_VCOM_TXPIN, gpioModePushPull, 1);

  // Configure VCOM receive pin as input
  GPIO_PinModeSet(BSP_VCOM_RXPORT, BSP_VCOM_RXPIN, gpioModeInput, 0);

  // Enable VCOM connection to board controller
  GPIO_PinModeSet(BSP_VCOM_ENABLE_PORT, BSP_VCOM_ENABLE_PIN, gpioModePushPull, 1);
}

/*
 * Initialize USART2 for VCOM communication
 */
void usart_init(void)
{
  // Enable USART2 clock
  CMU_ClockEnable(cmuClock_USART2, true);

  // Default asynchronous initializer (115.2 Kbps, 8N1, no flow control)
  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;

  // Configure and enable USART2
  USART_InitAsync(USART2, &init);

  // Configure routing for VCOM pins
  USART2->ROUTELOC0 = BSP_VCOM_RX_LOCATION | BSP_VCOM_TX_LOCATION;
  USART2->ROUTEPEN |= USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;

  // Enable NVIC interrupts for USART2
  NVIC_ClearPendingIRQ(USART2_RX_IRQn);
  NVIC_EnableIRQ(USART2_RX_IRQn);
  NVIC_ClearPendingIRQ(USART2_TX_IRQn);
  NVIC_EnableIRQ(USART2_TX_IRQn);

  // Initialize GPIO for VCOM
  init_gpio();

  // Enable RX data valid interrupt
  USART_IntEnable(USART2, USART_IEN_RXDATAV);
}

/*
 * Register RX callback function
 */
void usart_set_rx_callback(usart_rx_callback_t cb)
{
  rx_callback = cb;
}

/*
 * Register TX callback function
 */
void usart_set_tx_callback(usart_tx_callback_t cb)
{
  tx_callback = cb;
}

/*
 * Transmit a single byte (non-blocking)
 */
void usart_send(uint8_t data)
{
  // Enable TX buffer level interrupt
  USART_IntEnable(USART2, USART_IEN_TXBL);

  // Write data to TX register
  USART2->TXDATA = data;
}

/*
 * USART2 RX interrupt handler
 */
void USART2_RX_IRQHandler(void)
{
  // Read received data
  uint8_t data = USART2->RXDATA;

  // Call RX callback if registered
  if (rx_callback != NULL) {
    rx_callback(data);
  }
}

/*
 * USART2 TX interrupt handler
 */
void USART2_TX_IRQHandler(void)
{
  // Clear TX buffer level interrupt
  USART_IntClear(USART2, USART_IF_TXBL);

  // Disable TX buffer level interrupt
  USART_IntDisable(USART2, USART_IEN_TXBL);

  // Call TX callback if registered
  if (tx_callback != NULL) {
    tx_callback();
  }
}

/*
 * USART printf
 * */
int usart_printf(const char *format, ...) {
    char buffer[USART_TX_BUFFER_SIZE];
    va_list args;
    va_start(args, format);


    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (len > 0) {
        for (int i = 0; i < len; i++) {
            usart_send((uint8_t)buffer[i]);
        }
    }

    return len;
}
