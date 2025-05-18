/***************************************************************************//**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include <nv_usart.h>
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_eusart.h"
#include "em_gpio.h"
#include "sl_sleeptimer.h"

/***************************************************************************//**
 * Static Function.
 ******************************************************************************/


/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
  usart_init();
}

/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
  char* msg = "hello";
  for(int i=0; msg[i]!='\0';i++){
      usart_send(msg[i]);
  }
  sl_sleeptimer_delay_millisecond(1000);//1s
}

