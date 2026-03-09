/********************************** (C) COPYRIGHT *******************************
* File Name     : USBHID.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/02/01
* Description   : USB HID APIs
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "system.h"
#include "usb.h"
#include "usbconstant.h"
#include "usbhandler.h"
#include "usbhid.h"

static __xdata uint8_t usbhid_mouseReport[USBHID_MOUSE_REPORT_SIZE];
static __xdata uint8_t usbhid_consumerReport[USBHID_CONSUMER_REPORT_SIZE];
static __xdata uint8_t usbhid_keyboardReport[USBHID_KEYBOARD_REPORT_SIZE];
static __xdata epOutHandler_t epOutHandlerPtr;

// Flag of whether upload pointer is busy
static volatile __xdata uint8_t ep1InBusy;
static void usbhid_ep1SendReport(__xdata uint8_t *report, uint8_t length);

void usbhid_initialise(void) {

    usbhandler_deviceConfig();
    usbhandler_endPointConfig();

    memset(usbhid_mouseReport, 0, sizeof(usbhid_mouseReport));
    memset(usbhid_consumerReport, 0, sizeof(usbhid_consumerReport));
    memset(usbhid_keyboardReport, 0, sizeof(usbhid_keyboardReport));
    ep1InBusy = 0;
    epOutHandlerPtr = NULL;

    usbhandler_interruptConfig();
}

void usbhid_deinitialise(void) {

    usbhandler_disableDevice();

    memset(usbhid_mouseReport, 0, sizeof(usbhid_mouseReport));
    memset(usbhid_consumerReport, 0, sizeof(usbhid_consumerReport));
    memset(usbhid_keyboardReport, 0, sizeof(usbhid_keyboardReport));
    
    ep1InBusy = 0;
    epOutHandlerPtr = NULL;
}

void usbhid_attachEPOutHandler(epOutHandler_t epOutHandlerFunction) {

    if (epOutHandlerFunction == NULL) {
        return;
    } else {
        epOutHandlerPtr = epOutHandlerFunction;
    }
}

void USB_EP1_IN(void) {
    UEP1_T_LEN = 0;
    UEP1_CTRL = (UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
    ep1InBusy = 0;
}

void USB_EP1_RESET(void) {
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;
    ep1InBusy = 0;
}

void USB_EP2_OUT(void) {

    if (epOutHandlerPtr != NULL) {
        epOutHandlerPtr(USB_RX_LEN, Ep2Buffer);    
    }

    UEP2_T_LEN = 0;
    UEP2_CTRL = (UEP2_CTRL & ~MASK_UEP_R_RES) | UEP_R_RES_ACK;
}

void USB_EP2_RESET(void) {
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
}

static void usbhid_ep1SendReport(__xdata uint8_t *report, uint8_t length) {
    
    if (UsbConfig == 0) {
        return;
    }

    if ((report == NULL) || (length == 0) || (length > DEFAULT_ENDP1_SIZE)) {
        return;
    }
    
    while (ep1InBusy != 0) {
        // ...
    }

    memcpy(Ep1Buffer, report, length);

    UEP1_T_LEN = length;
    UEP1_CTRL = (UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
    
    // upload data, respond ACK and set EP1 IN to Busy until complete
    ep1InBusy = 1;
}

void usbhid_mouseWheelRotateUp(void) {
    
    usbhid_mouseReport[0] = USBHID_MOUSE_REPORT_ID;
    usbhid_mouseReport[1] = 0;
    usbhid_mouseReport[2] = 0;
    usbhid_mouseReport[3] = 0;
    usbhid_mouseReport[4] = (uint8_t)(1);
    usbhid_ep1SendReport(usbhid_mouseReport, USBHID_MOUSE_REPORT_SIZE);
}

void usbhid_mouseWheelRotateDown(void) {
    
    usbhid_mouseReport[0] = USBHID_MOUSE_REPORT_ID;
    usbhid_mouseReport[1] = 0;
    usbhid_mouseReport[2] = 0;
    usbhid_mouseReport[3] = 0;
    usbhid_mouseReport[4] = (uint8_t)(-1);
    usbhid_ep1SendReport(usbhid_mouseReport, USBHID_MOUSE_REPORT_SIZE);
}

void usbhid_consumerMediaMute(void) {

    // Consumer key press
    usbhid_consumerReport[0] = USBHID_CONSUMER_REPORT_ID;
    usbhid_consumerReport[1] = CON_VOL_MUTE;
    usbhid_consumerReport[2] = 0;       
    usbhid_ep1SendReport(usbhid_consumerReport, USBHID_CONSUMER_REPORT_SIZE);

    // Consumer key release
    usbhid_consumerReport[0] = USBHID_CONSUMER_REPORT_ID;
    usbhid_consumerReport[1] = 0;
    usbhid_consumerReport[2] = 0;       
    usbhid_ep1SendReport(usbhid_consumerReport, USBHID_CONSUMER_REPORT_SIZE);
}

void usbhid_keyboardLockWorkstation(void) {

    // Keyboard key press
    usbhid_keyboardReport[0] = USBHID_KEYBOARD_REPORT_ID;
    usbhid_keyboardReport[1] = KBD_KEY_LEFT_GUI;
    usbhid_keyboardReport[2] = 0;
    usbhid_keyboardReport[3] = KBD_KEY_L;
    usbhid_ep1SendReport(usbhid_keyboardReport, USBHID_KEYBOARD_REPORT_SIZE);

    // Keyboard key release
    usbhid_keyboardReport[0] = USBHID_KEYBOARD_REPORT_ID;
    usbhid_keyboardReport[1] = 0;
    usbhid_keyboardReport[2] = 0;
    usbhid_keyboardReport[3] = 0;
    usbhid_ep1SendReport(usbhid_keyboardReport, USBHID_KEYBOARD_REPORT_SIZE);
}

void usbhib_nullHandler(void) {
    
}

