/********************************** (C) COPYRIGHT *******************************
* File Name     : MENU_HEALPER.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/04/08
* Description   : Tokenised Menu Handler - Helper Functions
*******************************************************************************/

#ifndef __CH554_MENU_HELPER_H__
#define __CH554_MENU_HELPER_H__

#include <stdint.h>
#include "ch554.h"

uint16_t menu_parseNumericalString(char *s);
void menu_dumpHex(const uint8_t *data, uint16_t startAddress, uint16_t length);

#endif // __CH554_MENU_HELPER_H__