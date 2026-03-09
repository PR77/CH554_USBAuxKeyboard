/********************************** (C) COPYRIGHT *******************************
* File Name     : USBHandler.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/02/01
* Description   : USB Device Core Handlers
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "usb.h"
#include "usbconstant.h"
#include "usbhandler.h"
#include "usbhid.h"

// References - ch55xduino
// Project Files (Github):  https://github.com/DeqingSun/ch55xduino/tree/ch55xduino
// License: https://github.com/DeqingSun/ch55xduino/tree/ch55xduino?tab=LGPL-2.1-1-ov-file#

__xdata __at (EP0_ADDR) uint8_t Ep0Buffer[DEFAULT_ENDP0_SIZE];
__xdata __at (EP1_ADDR) uint8_t Ep1Buffer[DEFAULT_ENDP1_SIZE];
__xdata __at (EP2_ADDR) uint8_t Ep2Buffer[DEFAULT_ENDP2_SIZE];

uint16_t SetupLen = 0;
uint8_t SetupReq = 0;
volatile uint8_t UsbConfig = 0;

__code uint8_t *pDescr;

inline void NOP_Process(void) { }

void USB_EP0_SETUP(void) {
    __data uint8_t len = USB_RX_LEN;

    if (len == (sizeof(USB_SETUP_REQ))) {
        SetupLen = ((uint16_t)UsbSetupBuf->wLengthH << 8) | (UsbSetupBuf->wLengthL);
        len = 0; // Default is success and upload 0 length
        SetupReq = UsbSetupBuf->bRequest;
        if ((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) { 
            // Not standard request here is the commnunication starts, refer to
            // usbFunctionSetup of USBtiny or usb_setup in usbtiny
            switch ((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK)) {
                case USB_REQ_TYP_VENDOR:
                    switch (SetupReq) {
                    default:
                        len = 0xFF; // command not supported
                        break;
                    }
                    break;
                case USB_REQ_TYP_CLASS:
                    switch (SetupReq) {
                    case HID_SET_IDLE:
                        len = 0;
                        break;
                    case HID_GET_IDLE:
                        Ep0Buffer[0] = 0;
                        len = 1;
                        break;
                    case HID_GET_PROTOCOL:
                        Ep0Buffer[0] = 0;
                        len = 1;
                        break;
                    case HID_SET_PROTOCOL:
                        len = 0;
                        break;
                    case HID_GET_REPORT:
                        Ep0Buffer[0] = 0;
                        len = 1;
                        break;
                    case HID_SET_REPORT:
                        SetupLen = ((uint16_t)UsbSetupBuf->wLengthH << 8) | UsbSetupBuf->wLengthL;
                        len = 0;
                        break;
                    default:
                        len = 0;
                        break;
                    }
                    break;
                default:
                    len = 0xFF; // command not supported
                    break;
            }
        } else { // Standard request
            switch (SetupReq) { // Request ccfType
            case USB_GET_DESCRIPTOR:
                switch (UsbSetupBuf->wValueH) {
                case USB_DESCR_TYP_DEVICE: // Device Descriptor
                    pDescr = (__code uint8_t *)DeviceDescriptor; // Put Device Descriptor into outgoing buffer
                    len = DeviceDescriptor.bLength;
                    break;
                case USB_DESCR_TYP_CONFIG: // Configure Descriptor
                    pDescr = (__code uint8_t *)ConfigurationDescriptor;
                    len = ConfigurationDescriptor.Config.wTotalLength;
                    break;
                case USB_DESCR_TYP_STRING:
                    if (UsbSetupBuf->wValueL == 0)
                        pDescr = (__code uint8_t *)LanguageDescriptor;
                    else if (UsbSetupBuf->wValueL == 1)
                        pDescr = (__code uint8_t *)ManufacturerDescriptor;
                    else if (UsbSetupBuf->wValueL == 2)
                        pDescr = (__code uint8_t *)ProductDescriptor;
                    else if (UsbSetupBuf->wValueL == 3)
                        pDescr = (__code uint8_t *)SerialDescriptor;
                    else if (UsbSetupBuf->wValueL == 4)
                        pDescr = (__code uint8_t *)InterfaceDescriptor;
                    else {
                        len = 0xff;
                        break;
                    }
                    len = pDescr[0];
                    break;
                case USB_DESCR_TYP_REPORT:
                    if (UsbSetupBuf->wIndexL == 0) {
                        pDescr = (__code uint8_t *)ReportDescriptorIN;
                        len = ConfigurationDescriptor.HID_Device0.wDescriptorLength;
                    } else if (UsbSetupBuf->wIndexL == 1) {
                        pDescr = (__code uint8_t *)ReportDescriptorOUT;
                        len = ConfigurationDescriptor.HID_Device1.wDescriptorLength;
                    } else {
                        len = 0xff;
                    }
                    break;
                default:
                    len = 0xff; // Unsupported descriptors or error
                    break;
                }
                if (len != 0xff) {
                    if (SetupLen > len) 
                        SetupLen = len; // Limit length
                    len = SetupLen >= DEFAULT_ENDP0_SIZE ? DEFAULT_ENDP0_SIZE : SetupLen; // transmit length for this packet
                    for (__data uint8_t i = 0; i < len; i++)
                        Ep0Buffer[i] = pDescr[i];
                    SetupLen -= len;
                    pDescr += len;
                }
                break;
            case USB_SET_ADDRESS:
                SetupLen = UsbSetupBuf->wValueL; // Save the assigned address
                break;
            case USB_GET_CONFIGURATION:
                Ep0Buffer[0] = UsbConfig;
                if (SetupLen >= 1) {
                    len = 1;
                }
                break;
            case USB_SET_CONFIGURATION:
                UsbConfig = UsbSetupBuf->wValueL;
                break;
            case USB_GET_INTERFACE:
                break;
            case USB_SET_INTERFACE:
                break;
            case USB_CLEAR_FEATURE: // Clear Feature
                if ((UsbSetupBuf->bRequestType & 0x1F) ==	USB_REQ_RECIP_DEVICE) { // Clear the device featuee.
                    if ((((uint16_t)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x01) {
                        if (ConfigurationDescriptor.Config.bmAttributes & 0x20) {
                            // wake up
                        } else 
                            len = 0xFF; // Failed
                    } else
                        len = 0xFF; // Failed
                } else if ((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) { // endpoint
                    switch (UsbSetupBuf->wIndexL) {
                    case 0x84:
                        UEP4_CTRL = UEP4_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
                        break;
                    case 0x04:
                        UEP4_CTRL = UEP4_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES) | UEP_R_RES_ACK;
                        break;
                    case 0x83:
                        UEP3_CTRL = UEP3_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
                        break;
                    case 0x03:
                        UEP3_CTRL = UEP3_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES) | UEP_R_RES_ACK;
                        break;
                    case 0x82:
                        UEP2_CTRL = UEP2_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
                        break;
                    case 0x02:
                        UEP2_CTRL = UEP2_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES) | UEP_R_RES_ACK;
                        break;
                    case 0x81:
                        UEP1_CTRL = UEP1_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
                        break;
                    case 0x01:
                        UEP1_CTRL = UEP1_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES) | UEP_R_RES_ACK;
                        break;
                    default:
                        len = 0xFF; // Unsupported endpoint
                        break;
                    }
                } else
                    len = 0xFF; // Unsupported for non-endpoint
                break;
            case USB_SET_FEATURE: // Set Feature
                if ((UsbSetupBuf->bRequestType & 0x1F) == USB_REQ_RECIP_DEVICE) { // Set  the device featuee.	
                    if ((((uint16_t)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x01) {
                        if (ConfigurationDescriptor.Config.bmAttributes & 0x20) {
                        // suspend
                        } else
                            len = 0xFF; // Failed
                    } else 
                        len = 0xFF; // Failed
                } else if ((UsbSetupBuf->bRequestType & 0x1F) == USB_REQ_RECIP_ENDP) { // endpoint
                    if ((((uint16_t)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x00) {
                        switch (((uint16_t)UsbSetupBuf->wIndexH << 8) | UsbSetupBuf->wIndexL) {
                        case 0x84:
                            UEP4_CTRL = UEP4_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL; // Set endpoint4 IN STALL
                            break;
                        case 0x04:
                            UEP4_CTRL = UEP4_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL; // Set endpoint4 OUT Stall
                            break;
                        case 0x83:
                            UEP3_CTRL = UEP3_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL; // Set endpoint3 IN STALL
                            break;
                        case 0x03:
                            UEP3_CTRL = UEP3_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL; // Set endpoint3 OUT Stall
                            break;
                        case 0x82:
                            UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL; // Set endpoint2 IN STALL
                            break;
                        case 0x02:
                            UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL; // Set endpoint2 OUT Stall
                            break;
                        case 0x81:
                            UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL; // Set endpoint1 IN STALL
                            break;
                        case 0x01:
                            UEP1_CTRL = UEP1_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL; // Set endpoint1 OUT Stall
                            break;
                        default:
                            len = 0xFF; // Failed
                            break;
                        }
                    } else
                        len = 0xFF; // Failed
                } else
                    len = 0xFF; // Failed
                break;
            case USB_GET_STATUS:
                Ep0Buffer[0] = 0x00;
                Ep0Buffer[1] = 0x00;
                if (SetupLen >= 2) {
                    len = 2;
                } else {
                    len = SetupLen;
                }
                break;
            default:
                len = 0xff; // Failed
                break;
            }
        }
    } else {
        len = 0xff; // Wrong packet length
    }

    if (len == 0xff) {
        SetupReq = 0xFF;
        UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL; // STALL
    } else if (len <= DEFAULT_ENDP0_SIZE) { // Tx data to host or send 0-length packet
        UEP0_T_LEN = len;
        UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; // Expect DATA1, Answer ACK
    } else {
        UEP0_T_LEN = 0; // Tx data to host or send 0-length packet
        UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; // Expect DATA1, Answer ACK
    }
}

void USB_EP0_IN(void) {
    __data uint8_t len;

    switch (SetupReq) {
        case USB_GET_DESCRIPTOR:
            len = (SetupLen >= DEFAULT_ENDP0_SIZE) ? DEFAULT_ENDP0_SIZE : SetupLen;
            for (__data uint8_t i = 0; i < len; i++) {
                Ep0Buffer[i] = pDescr[i];
            }
            SetupLen -= len;
            pDescr += len;
            UEP0_T_LEN = len;
            UEP0_CTRL ^= bUEP_T_TOG; // Switch between DATA0 and DATA1
            break;
        
        case USB_SET_ADDRESS:
            USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
            UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
            break;
        
        default:
            UEP0_T_LEN = 0; // End of transaction
            UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
            break;
    }
}

void USB_EP0_OUT(void) {
    UEP0_T_LEN = 0;
    UEP0_CTRL |= UEP_R_RES_ACK | UEP_T_RES_NAK; // Respond Nak
}

void USB_EP0_RESET(void) {
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
}

#pragma save
#pragma nooverlay
void usbhandler_USBInterrupt(void) __interrupt(INT_NO_USB) {

    if (UIF_TRANSFER) { // Dispatch to service functions
        __data uint8_t callIndex = (USB_INT_ST & MASK_UIS_ENDP);
        switch (USB_INT_ST & MASK_UIS_TOKEN) {
        case UIS_TOKEN_OUT:
            switch (callIndex) {
            case 0:
                EP0_OUT_Callback();
                break;
            case 1:
                EP1_OUT_Callback();
                break;
            case 2:
                EP2_OUT_Callback();
                break;
            case 3:
                EP3_OUT_Callback();
                break;
            case 4:
                EP4_OUT_Callback();
                break;
            default:
                break;
            }
            break;
        case UIS_TOKEN_SOF:
            switch (callIndex) {
            case 0:
                EP0_SOF_Callback();
                break;
            case 1:
                EP1_SOF_Callback();
                break;
            case 2:
                EP2_SOF_Callback();
                break;
            case 3:
                EP3_SOF_Callback();
                break;
            case 4:
                EP4_SOF_Callback();
                break;
            default:
                break;
            }
            break;
        case UIS_TOKEN_IN:
            switch (callIndex) {
            case 0:
                EP0_IN_Callback();
                break;
            case 1:
                EP1_IN_Callback();
                break;
            case 2:
                EP2_IN_Callback();
                break;
            case 3:
                EP3_IN_Callback();
                break;
            case 4:
                EP4_IN_Callback();
                break;
            default:
                break;
            }
            break;
        case UIS_TOKEN_SETUP:
            switch (callIndex) {
            case 0:
                EP0_SETUP_Callback();
                break;
            case 1:
                EP1_SETUP_Callback();
                break;
            case 2:
                EP2_SETUP_Callback();
                break;
            case 3:
                EP3_SETUP_Callback();
                break;
            case 4:
                EP4_SETUP_Callback();
                break;
            default:
                break;
            }
            break;
        }

        UIF_TRANSFER = 0; // Clear interrupt flag
    }

    // Device mode USB bus reset
    if (UIF_BUS_RST) {

        EP0_RESET_Callback();
        EP1_RESET_Callback();
        EP2_RESET_Callback();
        EP3_RESET_Callback();
        EP4_RESET_Callback();

        USB_DEV_AD   = 0x00;
        UIF_SUSPEND  = 0;
        UIF_TRANSFER = 0;
        UIF_BUS_RST  = 0;                                   // clear interrupt flag

        UsbConfig = 0;
    }

    // USB bus suspend / wake up
    if (UIF_SUSPEND) {
        UIF_SUSPEND = 0;
        if (USB_MIS_ST & bUMS_SUSPEND) {
            // Suspend enabled
        } else {                                            // Unexpected interrupt, not supposed to happen !
            USB_INT_FG = 0xFF;
        }
    }
}
#pragma restore

void usbhandler_deviceConfig(void) {

    USB_CTRL = (bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN); // USB device and internal pull-up enable
    USB_DEV_AD = 0x00;      								// Device address initialization

#if defined(__CH551_H__) || defined(__CH552_H__) || defined(__CH554_H__) || defined(__CH549_H__)
    UDEV_CTRL = (bUD_PD_DIS | bUD_PORT_EN);                 // Disable DP / DM pull-down resistor
#endif

#if FREQ_SYS <= 12000000       								
    USB_CTRL |= bUC_LOW_SPEED;
    UDEV_CTRL |= bUD_LOW_SPEED;                             // Select slow speed 1.5M mode
#else
    USB_CTRL &= ~bUC_LOW_SPEED;
    UDEV_CTRL &= ~bUD_LOW_SPEED;                            // Select full speed 12M mode, default mode
#endif
}

void usbhandler_endPointConfig() {
    UEP0_DMA = (uint16_t)Ep0Buffer;                         // Endpoint 0 data transfer address
    UEP0_CTRL = (UEP_R_RES_ACK | UEP_T_RES_NAK);

    UEP1_DMA = (uint16_t)Ep1Buffer;                         // Endpoint 1 data transfer address
    UEP1_CTRL = (bUEP_AUTO_TOG | UEP_T_RES_NAK); 
    UEP4_1_MOD = (UEP4_1_MOD & ~bUEP1_BUF_MOD);
    UEP4_1_MOD = (UEP4_1_MOD | bUEP1_TX_EN);                // Endpoint 1 data transfer TX

    UEP2_DMA = (uint16_t)Ep2Buffer;                         // Endpoint 2 data transfer address
    UEP2_CTRL = (bUEP_AUTO_TOG | UEP_R_RES_ACK); 
    UEP2_3_MOD = (UEP2_3_MOD & ~bUEP2_BUF_MOD);
    UEP2_3_MOD = (UEP2_3_MOD | bUEP2_RX_EN);                // Endpoint 2 data transfer RX
}

void usbhandler_interruptConfig(void) {
    
    // Enable device hang interrupt, USB transfer completion interrupt and USB bus reset interrupt
    USB_INT_EN |= (bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST);
    USB_INT_FG = 0xFF;                                      // Clear interrupt flag
    IE_USB = 1;                                             // Enable USB interrupt

    UEP0_T_LEN = 0;
    UEP1_T_LEN = 0;
    UEP2_T_LEN = 0;
}

void usbhandler_disableDevice(void) {
    
    USB_CTRL = 0;
    USB_INT_EN = 0;
    USB_INT_FG = 0xFF;                                      // Clear interrupt flag
    IE_USB = 0;                                             // Disable USB interrupt
}