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

// https://pid.codes/1209/2345/
#define USB_VENDOR_ID       0x1209
#define USB_PRODUCT_ID      0x2345
#define USB_DEVICE_VERSION  0x0100

// USB descriptor strings
#define LANGUAGE_ID         0x0409
#define MANUFACTURER_STR    'P','R','7','7'
#define PRODUCT_STR         'M', 'A', 'C', 'R', 'O', '.', 'P', 'A', 'D', '.', 'O', 'R', '2' 
#define INTERFACE_STR       'U','S','B'
#define SERIAL_STR          '2','6','.','0','0','0','0','0','0','0','1'

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