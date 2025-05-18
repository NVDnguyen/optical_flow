/*
 * bsp.h
 *
 *  Created on: May 18, 2025
 *      Author: nvd
 */

#ifndef BSP_H_
#define BSP_H_

// EUSART1 TX: PC01 (Expansion Header Pin 4)
#define BSP_BCC_TXPORT      gpioPortC
#define BSP_BCC_TXPIN       1

// EUSART1 RX: PC02 (Expansion Header Pin 6)
#define BSP_BCC_RXPORT      gpioPortC
#define BSP_BCC_RXPIN       2

// VCOM Enable: PB00 (WSTK P15)
#define BSP_BCC_ENABLE_PORT gpioPortB
#define BSP_BCC_ENABLE_PIN  0

#endif /* BSP_H_ */
