/********************************** (C) COPYRIGHT *******************************
* File Name     : USBConstant.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/02/01
* Description   : USB Device Descriptors and Configuration Structures
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "firmware_info.h"
#include "usbconstant.h"
#include "usb.h"

// References - ch55xduino
// Project Files (Github):  https://github.com/DeqingSun/ch55xduino/tree/ch55xduino
// License: https://github.com/DeqingSun/ch55xduino/tree/ch55xduino?tab=LGPL-2.1-1-ov-file#

/**
 * USBconstant.c USB descriptors and HID report descriptor of CH554_USBAuxKeyboard
 * from CH55xDuino generic_examples.
 */
__code USB_DEV_DESCR DeviceDescriptor = {
    .bLength            = sizeof(DeviceDescriptor),     // size of the descriptor in bytes: 18
    .bDescriptorType    = USB_DESCR_TYP_DEVICE,   		// device descriptor: 0x01
    .bcdUSB             = 0x0110,                       // USB specification: USB 1.1
    .bDeviceClass       = 0,                            // interface will define class
    .bDeviceSubClass    = 0,                            // unused
    .bDeviceProtocol    = 0,                            // unused
    .bMaxPacketSize0    = DEFAULT_ENDP0_SIZE,           // maximum packet size for Endpoint 0
    .idVendor           = USB_VENDOR_ID,                // VID
    .idProduct          = USB_PRODUCT_ID,               // PID
    .bcdDevice          = USB_DEVICE_VERSION,           // device version
    .iManufacturer      = 1,                            // index of Manufacturer String Descr
    .iProduct           = 2,                            // index of Product String Descriptor
    .iSerialNumber      = 3,                            // index of Serial Number String Descr
    .bNumConfigurations = 1                             // number of possible configurations
}; // DeviceDescriptor

/** Configuration descriptor structure. This descriptor, located in FLASH
 * memory, describes the usage of the device in one of its supported
 * configurations, including information about any device interfaces and
 * endpoints. The descriptor is read out by the USB host during the enumeration
 * process when selecting a configuration so that the host may correctly
 * communicate with the USB device.
 */
__code USB_Descriptor_Configuration_t ConfigurationDescriptor = {
    .Config = {
        .bLength            = sizeof(USB_CFG_DESCR),    // size of the descriptor in bytes
        .bDescriptorType    = USB_DESCR_TYP_CONFIG,     // configuration descriptor: 0x02
        .wTotalLength       = sizeof(ConfigurationDescriptor),
        .bNumInterfaces     = 2,                        // number of interfaces: 2
        .bConfigurationValue= 1,                        // value to select this configuration
        .iConfiguration     = 0,                        // no configuration string descriptor
        .bmAttributes       = 0x80,                     // attributes = bus powered
        .MaxPower           = USB_MAX_POWER_mA / 2      // in 2mA units
    },

    // HID_Interface0 - Mouse, Consumer + Keyboard
    .HID_Interface0 = {
        .bLength            = sizeof(USB_ITF_DESCR),    // size of the descriptor in bytes: 9
        .bDescriptorType    = USB_DESCR_TYP_INTERF,     // interface descriptor: 0x04
        .bInterfaceNumber   = 0,                        // number of this interface: 0
        .bAlternateSetting  = 0,                        // value used to select alternative setting
        .bNumEndpoints      = 1,                        // number of endpoints used: 1
        .bInterfaceClass    = USB_DEV_CLASS_HID,        // interface class: HID (0x03)
        .bInterfaceSubClass = 0,                        // composite HID
        .bInterfaceProtocol = 0,                        // mouse, consumer, keyboard
        .iInterface         = 4                         // interface string descriptor
    },

    .HID_Device0 = {
        .bLength            = sizeof(USB_HID_DESCR),    // size of the descriptor in bytes: 9
        .bDescriptorType    = USB_DESCR_TYP_HID,        // HID descriptor: 0x21
        .bcdHID             = 0x0110,                   // HID class spec version (BCD: 1.1)
        .bCountryCode       = 0,                        // country code: Not specific
        .bNumDescriptors    = 1,                        // number of report descriptors: 1
        .bDescriptorTypeX   = USB_DESCR_TYP_REPORT,     // descriptor type: report (0x22)
        .wDescriptorLength  = sizeof(ReportDescriptorIN)
    },

    .HID_ReportINEndpoint = {
        .bLength            = sizeof(USB_ENDP_DESCR),   // size of the descriptor in bytes: 7
        .bDescriptorType    = USB_DESCR_TYP_ENDP,       // endpoint descriptor: 0x05
        .bEndpointAddress   = USB_ENDP_ADDR_EP1_IN,     // endpoint: 1, direction: IN (0x81)
        .bmAttributes       = USB_ENDP_TYPE_INTER,      // transfer type: interrupt (0x03)
        .wMaxPacketSize     = DEFAULT_ENDP1_SIZE,       // max packet size
        .bInterval          = 10                        // polling intervall in ms
    },

    // HID_Interface1 - LEDs
    .HID_Interface1 = {
        .bLength            = sizeof(USB_ITF_DESCR),    // size of the descriptor in bytes: 9
        .bDescriptorType    = USB_DESCR_TYP_INTERF,     // interface descriptor: 0x04
        .bInterfaceNumber   = 1,                        // number of this interface: 1
        .bAlternateSetting  = 0,                        // value used to select alternative setting
        .bNumEndpoints      = 1,                        // number of endpoints used: 1
        .bInterfaceClass    = USB_DEV_CLASS_HID,        // interface class: HID (0x03)
        .bInterfaceSubClass = 0,                        // composite HID
        .bInterfaceProtocol = 0,                        // leds
        .iInterface         = 4                         // interface string descriptor
    },

    .HID_Device1 = {
        .bLength            = sizeof(USB_HID_DESCR),    // size of the descriptor in bytes: 9
        .bDescriptorType    = USB_DESCR_TYP_HID,        // HID descriptor: 0x21
        .bcdHID             = 0x0110,                   // HID class spec version (BCD: 1.1)
        .bCountryCode       = 0,                        // country code: Not specific
        .bNumDescriptors    = 1,                        // number of report descriptors: 1
        .bDescriptorTypeX   = USB_DESCR_TYP_REPORT,     // descriptor type: report (0x22)
        .wDescriptorLength  = sizeof(ReportDescriptorOUT)
    },

    .HID_ReportOUTEndpoint = {
        .bLength            = sizeof(USB_ENDP_DESCR),   // size of the descriptor in bytes: 7
        .bDescriptorType    = USB_DESCR_TYP_ENDP,       // endpoint descriptor: 0x05
        .bEndpointAddress   = USB_ENDP_ADDR_EP2_OUT,    // endpoint: 2, direction: OUT (0x02)
        .bmAttributes       = USB_ENDP_TYPE_INTER,      // transfer type: interrupt (0x03)
        .wMaxPacketSize     = DEFAULT_ENDP2_SIZE,       // max packet size
        .bInterval          = 10                        // polling intervall in ms
    }
}; // ConfigurationDescriptor

__code uint8_t ReportDescriptorIN[] = {

    // MOUSE
    0x05, 0x01,                                         // Usage Page (Generic Desktop)
    0x09, 0x02,                                         // Usage (Mouse)
    0xA1, 0x01,                                         // Collection (Application)
    0x85, 0x01,                                         //   Report ID (1)
    0x09, 0x01,                                         //   Usage (Pointer)
    0xA1, 0x00,                                         //   Collection (Physical)
    0x05, 0x09,                                         //     Buttons
    0x19, 0x01,                                         
    0x29, 0x03,                                         
    0x15, 0x00,                                         
    0x25, 0x01,                                         
    0x75, 0x01,                                         
    0x95, 0x03,                                         
    0x81, 0x02,                                         
    0x75, 0x05,                                         
    0x95, 0x01,                                         
    0x81, 0x03,                                         
    0x05, 0x01,                                         
    0x09, 0x30,                                         // X
    0x09, 0x31,                                         // Y
    0x09, 0x38,                                         // Wheel
    0x15, 0x81,                                         
    0x25, 0x7F,                                         
    0x75, 0x08,                                         
    0x95, 0x03,                                         
    0x81, 0x06,                                         // Relative
    0xC0,                                               // End Physical
    0xC0,                                               // End Application

    // CONSUMER
    0x05, 0x0C,                                         // Usage Page (Consumer)
    0x09, 0x01,                                         // Usage (Consumer Control)
    0xA1, 0x01,                                         // Collection (Application)
    0x85, 0x02,                                         //   Report ID (2)
    0x15, 0x00,                                         
    0x26, 0xFF, 0x03,                                   //   Logical Max (0x03FF)
    0x19, 0x00,                                         
    0x2A, 0xFF, 0x03,                                           
    0x75, 0x10,                                         
    0x95, 0x01,                                         
    0x81, 0x00,                                         //   Input (Consumer usage)
    0xC0,                                               // End Collection

    // KEYBOARD
    0x05, 0x01,                                         // Usage Page (Generic Desktop)
    0x09, 0x06,                                         // Usage (Keyboard)
    0xA1, 0x01,                                         // Collection (Application)
    0x85, 0x03,                                         //   Report ID (3)

    // --- Modifier keys (8 bits)
    0x05, 0x07,                                         //   Usage Page (Keyboard/Keypad)
    0x19, 0xE0,                                         //   Usage Minimum (Keyboard LeftControl)
    0x29, 0xE7,                                         //   Usage Maximum (Keyboard Right GUI)
    0x15, 0x00,                                         //   Logical Minimum (0)
    0x25, 0x01,                                         //   Logical Maximum (1)
    0x75, 0x01,                                         //   Report Size (1)
    0x95, 0x08,                                         //   Report Count (8)
    0x81, 0x02,                                         //   Input (Data, Variable, Absolute)

    // --- Reserved byte
    0x75, 0x08,                                         //   Report Size (8)
    0x95, 0x01,                                         //   Report Count (1)
    0x81, 0x01,                                         //   Input (Constant)

    // --- Key array (5 bytes)
    0x75, 0x08,                                         //   Report Size (8)
    0x95, 0x05,                                         //   Report Count (5)
    0x15, 0x00,                                         //   Logical Minimum (0)
    0x25, 0xA4,                                         //   Logical Maximum (164)
    0x05, 0x07,                                         //   Usage Page (Keyboard/Keypad)
    0x19, 0x00,                                         //   Usage Minimum (Reserved)
    0x29, 0xA4,                                         //   Usage Maximum (Keyboard Application)
    0x81, 0x00,                                         //   Input (Data, Array)
    0xC0                                                // End Collection
}; // ReportDescriptorIN

__code uint8_t ReportDescriptorOUT[] = {

    // LEDS
    0x06, 0x00, 0xFF,                                   // Usage Page (Vendor Defined)
    0x09, 0x01,                                         // Usage
    0xA1, 0x01,                                         // Collection (Application)
    
    // --- LED output report (host to device)
    0x85, 0x04,                                         //   Report ID = 4
    0x09, 0x06,                                         //   Usage Output
    0x15, 0x00,                                         //   Logical Min (0)
    0x25, 0xFF,                                         //   Logical Max (255)
    0x75, 0x08,                                         //   Report Size = 8 bits
    0x95, 0x01,                                         //   Report Count = 1 byte
    0x91, 0x02,                                         //   Output (Data,Var,Abs)
    0xC0                                                // End Collection
}; // ReportDescriptorOUT

// String Descriptors
__code uint16_t LanguageDescriptor[] = {
    ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(LanguageDescriptor),
    LANGUAGE_ID
};

__code uint16_t ManufacturerDescriptor[] = {
	((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(ManufacturerDescriptor), 
    MANUFACTURER_STR
};

__code uint16_t ProductDescriptor[] = {
	((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(ProductDescriptor), 
	PRODUCT_STR
};

__code uint16_t InterfaceDescriptor[] = {
	((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(InterfaceDescriptor), 
    INTERFACE_STR
};

__code uint16_t SerialDescriptor[] = {
	((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(SerialDescriptor),
	SERIAL_STR
};


