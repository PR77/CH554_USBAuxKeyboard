/********************************** (C) COPYRIGHT *******************************
* File Name     : USBCDC.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/02/01
* Description   : USB CDC APIs
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ch554.h"
#include "system.h"
#include "usb.h"
#include "usbconstant.h"
#include "usbhandler.h"
#include "usbcdc.h"
#include "tick.h"

static volatile __data uint8_t usbcdc_transmitByteIndex;
static volatile __data bool usbcdc_transmitLastPacketWasFull;
static volatile __data uint8_t usbcdc_receiveByteCount;
static volatile __data uint8_t usbcdc_receiveByteIndex;
static volatile __data bool usbcdc_dtrAsserted;

static __xdata USB_CDC_LineCodingDesc lineCodingDescriptor = {
    .baudrate = 115200,       // baudrate 115200
    .stopbits = 0,            // 1 stopbit
    .parity   = 0,            // no parity
    .databits = 8             // 8 databits
};

void usbcdc_initialise(void) {

    usbhandler_deviceConfig();
    usbhandler_endPointConfig();
    usbhandler_interruptConfig();

    usbcdc_transmitByteIndex = 0;
    usbcdc_transmitLastPacketWasFull = false;
    usbcdc_receiveByteCount = 0;
    usbcdc_receiveByteIndex = 0;
}

void usbcdc_deinitialise(void) {

    usbhandler_disableDevice();
}

void USB_EP1_IN(void) {
    UEP1_T_LEN = 0;
    UEP1_CTRL = (UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
}

void USB_EP1_RESET(void) {
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;
}

// Endpoint 2 IN handler (bulk data transferred to host)
void USB_EP2_IN(void) {
    UEP2_T_LEN = 0;
    UEP2_CTRL = (UEP2_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
}

// Endpoint 2 OUT handler (bulk data transferred from host)
void USB_EP2_OUT(void) {

    if (U_TOG_OK && USB_RX_LEN) {
        usbcdc_receiveByteCount = USB_RX_LEN;
        usbcdc_receiveByteIndex = 0;

        // Block receiving more data with UEP_R_RES_NAK until EP2 buffer has
        // been read. Read handler will ACK the transfer.
        UEP2_CTRL = (UEP2_CTRL & ~MASK_UEP_R_RES) | UEP_R_RES_NAK;
    }
}

void USB_EP2_RESET(void) {

    usbcdc_transmitByteIndex = 0;
    usbcdc_transmitLastPacketWasFull = false;
    usbcdc_receiveByteCount = 0;
    usbcdc_receiveByteIndex = 0;

    UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;
}

void usbcdc_resetCDCParameters(void) {
}

void usbcdc_setLineCodingHandler(void) {
    uint8_t lineCodingPayloadSize = (sizeof(USB_CDC_LineCodingDesc) <= USB_RX_LEN) ? sizeof(USB_CDC_LineCodingDesc) : USB_RX_LEN;
    // Limited lineCodingPayloadSize to the size of the structure of the payload
    // length if smaller.

    memcpy((uint8_t *)&lineCodingDescriptor, Ep0Buffer, lineCodingPayloadSize);
}

uint8_t usbcdc_getLineCodingHandler(void) {
    uint8_t lineCodingPayloadSize = sizeof(USB_CDC_LineCodingDesc);

    memcpy(Ep0Buffer, (uint8_t *)&lineCodingDescriptor, lineCodingPayloadSize);

    return (lineCodingPayloadSize);
}

uint8_t usbcdc_setControlLineStateHandler(uint8_t controlLineState) {
    usbcdc_dtrAsserted = (controlLineState & 0x01) ? true : false;
    return (0);
}

bool usbcdc_getDtrState(void) {
    return (usbcdc_dtrAsserted);
}

void usbcdc_printCharacter(char character) {
    
    while ((UEP2_CTRL & MASK_UEP_T_RES) == UEP_T_RES_ACK) {
        // wait for previous IN transfer to complete
    }

    do {
        Ep2Buffer[64 + usbcdc_transmitByteIndex++] = character;

        if (usbcdc_transmitByteIndex == DEFAULT_ENDP2_SIZE) {
            UEP2_T_LEN = usbcdc_transmitByteIndex;
            usbcdc_transmitLastPacketWasFull = true;
            UEP2_CTRL = (UEP2_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
            usbcdc_transmitByteIndex = 0;
        }
    } while (0);
}

void usbcdc_flushTransmitBuffer(void) {

    // Determine if a zero-length packet (ZLP) needs to be sent.
    
    while ((UEP2_CTRL & MASK_UEP_T_RES) == UEP_T_RES_ACK) {
        // wait for previous IN transfer to complete
        ;
    }

    if (usbcdc_transmitByteIndex > 0) {
        // Partial buffer as usbcdc_transmitByteIndex is > 0. Send it as a short
        // packet. Since it's < max size (DEFAULT_ENDP2_SIZE), this alone terminates
        // the transfer for the host - no ZLP needed.
        UEP2_T_LEN = usbcdc_transmitByteIndex;
        usbcdc_transmitLastPacketWasFull = false;
        UEP2_CTRL = (UEP2_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
        usbcdc_transmitByteIndex = 0;
    } else if (usbcdc_transmitLastPacketWasFull == true) {
        // Nothing pending, but the last packet we sent was exactly max-size
        // therefore host is still waiting to see if more is coming. Send an
        // explicit zero-length packet (ZLP) to close the transfer.
        UEP2_T_LEN = 0;
        usbcdc_transmitLastPacketWasFull = 0;
        UEP2_CTRL = (UEP2_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
    }
}

uint16_t usbcdc_getCharacter(uint8_t timeout) {
    uint16_t receivedData = CDC_RECEIVE_NO_DATA_AVAIL;
    uint32_t previousCountTimeout = tick_get1msTimerCount();

    // If receive buffer is empty, then block for timeout period only if
    // timeout period is !0.
    // If timeout occurs, then break out or report CDC_RECEIVE_TIMEOUT.
    if (timeout != 0) {
        while (!usbcdc_receiveByteCount) {

            if ((uint8_t)(tick_get1msTimerCount() - previousCountTimeout) > timeout) {
                return (CDC_RECEIVE_TIMEOUT);
            }
        }
    }

    do {
        if (usbcdc_receiveByteCount == 0) {
            break;
        }
        
        receivedData = Ep2Buffer[usbcdc_receiveByteIndex++];
        if (--usbcdc_receiveByteCount == 0) {
            // Unblock receiving more data now with UEP_R_RES_ACK as all data
            // has been read.
            UEP2_CTRL = (UEP2_CTRL & ~MASK_UEP_R_RES) | UEP_R_RES_ACK;
        }
    } while (0);

    return (receivedData);
}
