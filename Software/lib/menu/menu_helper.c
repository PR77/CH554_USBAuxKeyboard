/********************************** (C) COPYRIGHT *******************************
* File Name     : MENU_HEALPER.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/04/08
* Description   : Tokenised Menu Handler - Helper Functions
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "serial.h"
#include "menu_helper.h"

uint16_t menu_parseNumericalString(char *s) {

    // Thanks ChatGPT
    uint16_t result = 0;
    uint8_t base = 10;

    // Detect hex prefix
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        base = 16;
        s += 2;
    }

    while (*s) {
        uint8_t digit;

        if (*s >= '0' && *s <= '9') {
            digit = *s - '0';
        }
        else if (base == 16 && *s >= 'A' && *s <= 'F') {
            digit = *s - 'A' + 10;
        }
        else if (base == 16 && *s >= 'a' && *s <= 'f') {
            digit = *s - 'a' + 10;
        }
        else {
            break; // stop on invalid character
        }

        result = (result * base) + digit;
        s++;
    }

    return result;
}

void menu_dumpHex(const uint8_t *data, uint16_t startAddress, uint16_t length) 
{
    uint16_t offset = 0;

    while (offset < length) {

        // --- Print address ---
        serial_printHexWord(startAddress + offset);
        serial_printString(": ");

        // --- Print hex bytes ---
        uint8_t i;
        for (i = 0; i < 16; i++) {
            if (offset + i < length) {
                serial_printHexByte(data[offset + i]);
                serial_printCharacter(' ');
            } else {
                // padding for incomplete line
                serial_printString("   ");
            }
        }

        // --- ASCII separator ---
        serial_printString(" |");

        // --- Print ASCII representation ---
        for (i = 0; i < 16; i++) {
            if (offset + i < length) {
                uint8_t c = data[offset + i];

                if (c >= 32 && c <= 126) {
                    serial_printCharacter(c);
                } else {
                    serial_printCharacter('.');
                }
            } else {
                serial_printCharacter(' ');
            }
        }

        serial_printString("|\n");

        offset += 16;
    }
}