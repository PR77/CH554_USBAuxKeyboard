/********************************** (C) COPYRIGHT *******************************
* File Name     : USBConstant.h
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/02/01
* Description   : USB Device Descriptors and Configuration Structures
*******************************************************************************/

#ifndef __CH554_USB_CONSTANT_H__
#define __CH554_USB_CONSTANT_H__

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "usb.h"
#include "usbhandler.h"
#include "usbhid.h"

#define USB_MAX_POWER_mA    500

/** Type define for the device configuration descriptor structure. This must be
 * defined in the application code, as the configuration descriptor contains
 * several sub-descriptors which vary between devices, and which describe the
 * device's usage to the host.
 */
typedef struct {
  USB_CFG_DESCR Config;
  USB_ITF_DESCR HID_Interface0;
  USB_HID_DESCR HID_Device0;
  USB_ENDP_DESCR HID_ReportINEndpoint;
  USB_ITF_DESCR HID_Interface1;
  USB_HID_DESCR HID_Device1;
  USB_ENDP_DESCR HID_ReportOUTEndpoint;
} USB_Descriptor_Configuration_t;

extern __code USB_DEV_DESCR DeviceDescriptor;
extern __code USB_Descriptor_Configuration_t ConfigurationDescriptor;
extern __code uint8_t ReportDescriptorIN[];
extern __code uint8_t ReportDescriptorOUT[];
extern __code uint16_t LanguageDescriptor[];
extern __code uint16_t ManufacturerDescriptor[];
extern __code uint16_t ProductDescriptor[];
extern __code uint16_t InterfaceDescriptor[];
extern __code uint16_t SerialDescriptor[];

#endif // __CH554_USB_CONSTANT_H__