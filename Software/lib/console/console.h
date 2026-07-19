/********************************** (C) COPYRIGHT *******************************
* File Name     : CONSOLE.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/07/10
* Description   : Console Common
*******************************************************************************/

#ifndef __CH554_CONSOLE_H__
#define __CH554_CONSOLE_H__

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ch554.h"
#include "console_cfg.h"

#if defined(CONSOLE_OVER_SERIAL)
#define CONSOLE_RECEIVE_TIMEOUT         SERIAL_RECEIVE_TIMEOUT
#define CONSOLE_RECEIVE_NO_DATA_AVAIL   SERIAL_RECEIVE_NO_DATA_AVAIL
#define CONSOLE_RECEIVE_DATA_AVAIL      SERIAL_RECEIVE_DATA_AVAIL
#elif defined(CONSOLE_OVER_USB)
#define CONSOLE_RECEIVE_TIMEOUT         CDC_RECEIVE_TIMEOUT
#define CONSOLE_RECEIVE_NO_DATA_AVAIL   CDC_RECEIVE_NO_DATA_AVAIL
#define CONSOLE_RECEIVE_DATA_AVAIL      CDC_RECEIVE_DATA_AVAIL
#else 
#error No valid definition of CONSOLE_OVER_SERIAL or CONSOLE_OVER_USB
#endif // CONSOLE_OVER_SERIAL else CONSOLE_OVER_USB

#ifndef CONSOLE_COLUMN_WIDTH
#define CONSOLE_COLUMN_WIDTH            80
#endif

void console_printCharacter(char character);
uint16_t console_getCharacter(uint32_t timeout);
void console_printStringPadded(char *string, uint8_t stringPaddingSize);
void console_printStringTitle(char *string, uint8_t totalLineLength, char paddingCharacter, bool addNewLine);
void console_printString(char* string);
void console_printHexByte(uint8_t value);
void console_printHexWord(uint16_t value);

#endif // __CH554_CONSOLE_H__