/*
 * bsp.h
 *
 *  Created on: May 18, 2025
 *      Author: nvd
 *
 * Board Support Package (BSP) header file for Silicon Labs EFR32MG12 Radio Board
 * (BRD4161A) combined with Wireless Starter Kit Mainboard, using the
 * EFR32MG12P432F1024GL125 device. Defines GPIO pin assignments and USART2
 * routing for TX (PA6) and RX (PA7).
 *
 * Copyright 2025 Silicon Laboratories Inc. www.silabs.com
 * Licensed under the Zlib license, see main_xg1_xg12_xg13_xg14.c for details.
 */

#ifndef BSP_H
#define BSP_H

#include "em_gpio.h"
#include "em_usart.h"

// VCOM USART2 TX pin configuration
#define BSP_VCOM_TXPORT           gpioPortA
#define BSP_VCOM_TXPIN            6
#define BSP_VCOM_TX_LOCATION      _USART_ROUTELOC0_TXLOC_LOC0

// VCOM USART2 RX pin configuration
#define BSP_VCOM_RXPORT           gpioPortA
#define BSP_VCOM_RXPIN            7
#define BSP_VCOM_RX_LOCATION      _USART_ROUTELOC0_RXLOC_LOC0

// VCOM enable pin configuration (for board controller communication)
#define BSP_VCOM_ENABLE_PORT      gpioPortA
#define BSP_VCOM_ENABLE_PIN       5  // Typical pin for VCOM enable, as per example

#endif /* BSP_H */
