/********************************** (C) COPYRIGHT *******************************
* File Name     : USB_CFG.h
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/02/01
* Description   : Config for USB Device Descriptors and Configuration Structures
*******************************************************************************/

#ifndef __CH554_USB_CFG_H__
#define __CH554_USB_CFG_H__

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "usb.h"

#define EP0_ADDR            0x0300
#define EP1_ADDR            (EP0_ADDR + DEFAULT_ENDP0_SIZE)
#define EP2_ADDR            (EP1_ADDR + DEFAULT_ENDP1_SIZE)

#ifndef USB_MAX_POWER_mA
#define USB_MAX_POWER_mA    500
#endif

#endif // __CH554_USB_CFG_H__