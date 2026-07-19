/********************************** (C) COPYRIGHT *******************************
* File Name     : USBCDC.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/07/04
* Description   : USB CDC APIs
*******************************************************************************/

#ifndef __CH554_USB_CDC_H__
#define __CH554_USB_CDC_H__

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ch554.h"
#include "usb.h"
#include "usbconstant.h"
#include "usbhandler.h"

#define CDC_RECEIVE_TIMEOUT         (uint16_t)UINT16_MAX
#define CDC_RECEIVE_NO_DATA_AVAIL   (uint16_t)(UINT16_MAX - 1)
#define CDC_RECEIVE_DATA_AVAIL      (uint16_t)(UINT16_MAX - 2)

#define LOW_BYTE(x)                 ((uint8_t)((x) & 0xFF))
#define HIGH_BYTE(x)                ((uint8_t)(((x) >> 8) & 0xFF))

void USB_EP1_IN(void);
void USB_EP1_RESET(void);
void USB_EP2_IN(void);
void USB_EP2_OUT(void);
void USB_EP2_RESET(void);

void usbcdc_initialise(void);
void usbcdc_deinitialise(void);
void usbcdc_resetCDCParameters(void);
void usbcdc_setLineCodingHandler(void);
uint8_t usbcdc_getLineCodingHandler(void);
uint8_t usbcdc_setControlLineStateHandler(uint8_t controlLineState);

bool usbcdc_getDtrState(void);
void usbcdc_printCharacter(char character);
void usbcdc_flushTransmitBuffer(void);
uint16_t usbcdc_getCharacter(uint8_t timeout);

#endif // __CH554_USB_CDC_H__