/********************************** (C) COPYRIGHT *******************************
* File Name     : CONSOLE_CFG.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/07/10
* Description   : Console Configuration
*******************************************************************************/

#ifndef __CH554_CONSOLE_CFG_H__
#define __CH554_CONSOLE_CFG_H__

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"

#if defined(CONSOLE_OVER_SERIAL)
#include "serial_0.h"
#include "serial_1.h"
#endif // CONSOLE_OVER_SERIAL

#if defined(CONSOLE_OVER_USB)
#include "device/usbcdc.h"
#endif // CONSOLE_OVER_USB

#define CONSOLE_DECIMAL_DIGITS_MAX  6   // 5 digits and NULL.
#define CONSOLE_U16_STR_DIGITS      5   // digits in max uint16_t value: 65535

#ifndef CONSOLE_PORT_PUTCHR
#if defined(CONSOLE_OVER_SERIAL)
#define CONSOLE_PORT_PUTCHR(x)      serial_sendByteSerial1(x)
#elif defined(CONSOLE_OVER_USB)
#define CONSOLE_PORT_PUTCHR(x)      usbcdc_printCharacter(x)
#else 
#error No valid definition of CONSOLE_PORT_PUTCHR
#endif // CONSOLE_OVER_SERIAL else CONSOLE_OVER_USB
#endif

#ifndef CONSOLE_PORT_FLUSH
#if defined(CONSOLE_OVER_SERIAL)
#define CONSOLE_PORT_FLUSH(x)
#elif defined(CONSOLE_OVER_USB)
#define CONSOLE_PORT_FLUSH(x)       usbcdc_flushTransmitBuffer(x)
#else 
#error No valid definition of CONSOLE_PORT_FLUSH
#endif // CONSOLE_OVER_SERIAL else CONSOLE_OVER_USB
#endif

#ifndef CONSOLE_PORT_GETCHR
#if defined(CONSOLE_OVER_SERIAL)
#define CONSOLE_PORT_GETCHR(x)      serial_getByteSerial1(x)
#elif defined(CONSOLE_OVER_USB)
#define CONSOLE_PORT_GETCHR(x)      usbcdc_getCharacter(x)
#else 
#error No valid definition of CONSOLE_PORT_GETCHR
#endif // CONSOLE_OVER_SERIAL else CONSOLE_OVER_USB
#endif

#endif // __CH554_CONSOLE_CFG_H__