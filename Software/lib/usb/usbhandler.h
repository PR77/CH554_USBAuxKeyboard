/********************************** (C) COPYRIGHT *******************************
* File Name     : USBHandler.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/02/01
* Description   : USB Device Core Handlers
*******************************************************************************/

#ifndef __CH554_USB_HANDLER_H__
#define __CH554_USB_HANDLER_H__

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "usb.h"
#include "usbconstant.h"
#include "usbhid.h"

#define EP0_ADDR            0x0380
#define EP1_ADDR            0x03C0
#define EP2_ADDR            0x03E0

extern __xdata __at (EP0_ADDR) uint8_t Ep0Buffer[DEFAULT_ENDP0_SIZE];
extern __xdata __at (EP1_ADDR) uint8_t Ep1Buffer[DEFAULT_ENDP1_SIZE];
extern __xdata __at (EP2_ADDR) uint8_t Ep2Buffer[DEFAULT_ENDP2_SIZE];

extern volatile uint8_t UsbConfig;

#define UsbSetupBuf ((PUSB_SETUP_REQ)Ep0Buffer)

// Out
#define EP0_OUT_Callback USB_EP0_OUT
#define EP1_OUT_Callback NOP_Process
#define EP2_OUT_Callback USB_EP2_OUT
#define EP3_OUT_Callback NOP_Process
#define EP4_OUT_Callback NOP_Process

// SOF
#define EP0_SOF_Callback NOP_Process
#define EP1_SOF_Callback NOP_Process
#define EP2_SOF_Callback NOP_Process
#define EP3_SOF_Callback NOP_Process
#define EP4_SOF_Callback NOP_Process

// IN
#define EP0_IN_Callback USB_EP0_IN
#define EP1_IN_Callback USB_EP1_IN
#define EP2_IN_Callback NOP_Process
#define EP3_IN_Callback NOP_Process
#define EP4_IN_Callback NOP_Process

// SETUP
#define EP0_SETUP_Callback USB_EP0_SETUP
#define EP1_SETUP_Callback NOP_Process
#define EP2_SETUP_Callback NOP_Process
#define EP3_SETUP_Callback NOP_Process
#define EP4_SETUP_Callback NOP_Process

// RESET
#define EP0_RESET_Callback USB_EP0_RESET
#define EP1_RESET_Callback USB_EP1_RESET
#define EP2_RESET_Callback USB_EP2_RESET
#define EP3_RESET_Callback NOP_Process
#define EP4_RESET_Callback NOP_Process

void usbhandler_USBInterrupt(void) __interrupt(INT_NO_USB);
void usbhandler_deviceConfig(void);
void usbhandler_endPointConfig(void);
void usbhandler_interruptConfig(void);
void usbhandler_disableDevice(void);

#endif // __CH554_USB_HANDLER_H__