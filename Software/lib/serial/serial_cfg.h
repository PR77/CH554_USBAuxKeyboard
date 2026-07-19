/********************************** (C) COPYRIGHT *******************************
* File Name     : SERIAL_CFG.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2024/12/17
* Description   : 8051 UART Configuration
*******************************************************************************/

#ifndef __CH554_SERIAL_CFG_H__
#define __CH554_SERIAL_CFG_H__

#include <stdint.h>
#include "ch554.h"

#ifndef SERIAL_BAUD_RATE
#define SERIAL_BAUD_RATE                57600
#endif

#define SERIAL_RECEIVE_TIMEOUT          (uint16_t)UINT16_MAX
#define SERIAL_RECEIVE_NO_DATA_AVAIL    (uint16_t)(UINT16_MAX - 1)
#define SERIAL_RECEIVE_DATA_AVAIL       (uint16_t)(UINT16_MAX - 2)

#endif // __CH554_SERIAL_CFG_H__