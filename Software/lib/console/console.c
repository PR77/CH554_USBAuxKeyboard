/********************************** (C) COPYRIGHT *******************************
* File Name     : CONSOLE.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/07/10
* Description   : Console Common
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "console.h"
#include "console_cfg.h"

// Buffer for int16_t / uint16_t to decimal string converion.
__xdata uint8_t console_conversionBuffer[CONSOLE_DECIMAL_DIGITS_MAX];

// HEX encoding table
static const uint8_t console_hexTable[] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F'
};

void console_printCharacter(char character) {
    CONSOLE_PORT_PUTCHR(character);
    CONSOLE_PORT_FLUSH();
}

uint16_t console_getCharacter(uint32_t timeout) {
    return (CONSOLE_PORT_GETCHR(timeout));
}

void console_printStringPadded(char *string, uint8_t stringPaddingSize) {
    uint8_t stringIndex = 0;

    if (string != NULL) {
        while (string[stringIndex] != '\0') {
            // Repeat until string terminator reached printing each character
            // on the console.
            CONSOLE_PORT_PUTCHR(string[stringIndex]);
            stringIndex++;
        }
    }

    // If string == NULL still do padding to ensure user interface consistency.
    while (stringIndex < stringPaddingSize) {
        // Now if there are still character to print but the padding number
        // has been reached, just print 'SPACE' characters.
        CONSOLE_PORT_PUTCHR(' ');
        stringIndex++;
    }

    CONSOLE_PORT_FLUSH();
}

void console_printStringTitle(char *string, uint8_t totalLineLength, char paddingCharacter, bool addNewLine) {
    uint8_t numberOfPadCharacters = 0;
    uint8_t stringLength = strlen(string);
    
    if (string != NULL) {
        if (stringLength < totalLineLength) {
            // Determine number of needed padding characters. If string is NULL, then
            // fill whole line with the padding character.
            numberOfPadCharacters = totalLineLength - stringLength - 1;
        }
        
        while (*string) {
            // Repeat until string terminator reached printing each character
            // on the console.
            CONSOLE_PORT_PUTCHR(*string++);
        }
        
        CONSOLE_PORT_PUTCHR(' ');
    } else {
        numberOfPadCharacters = totalLineLength;
    }
    
    for (uint8_t i = 0; i < numberOfPadCharacters; i++) {
        CONSOLE_PORT_PUTCHR(paddingCharacter);
    }

    if (addNewLine) {
        CONSOLE_PORT_PUTCHR('\n'); 
    }

    CONSOLE_PORT_FLUSH();
}

void console_printString(char* string) {

    if (string == NULL) {
        return;
    }
    
    while (*string) {
        // Repeat until string terminator reached printing each character
        // on the console.
        CONSOLE_PORT_PUTCHR(*string++);
    }

    CONSOLE_PORT_FLUSH();
}

void console_printUnsignedDecimalWord(uint16_t value) {

    // Writes exactly CONSOLE_U16_STR_DIGITS zero-padded characters into buffer, 
    // followed by a null terminator. Digits are extracted least-significant-first and
    // written directly into their final position, so no reversal pass is required.
    //
    // 'i' counts down from CONSOLE_U16_STR_DIGITS to 1, decremented in the loop's third
    // clause and used after subtracting 1 each time - so it only ever indexes buffer 
    // as [0, digits-1].
    for (uint8_t i = CONSOLE_U16_STR_DIGITS; i > 0; i--) {
        console_conversionBuffer[i - 1] = (char)('0' + (uint8_t)(value % 10));
        value /= 10; 
    }

    console_conversionBuffer[CONSOLE_U16_STR_DIGITS] = 0;
    console_printString(console_conversionBuffer);
}

void console_printSignedDecimalWord(int16_t value) {

    if (value < 0) {
        console_conversionBuffer[0] = '-';

        // Convert value to positive to ensure the decimal conversion logic works.
        value = -value;
    } else {
        console_conversionBuffer[0] = '+';
    }
    
    // Writes exactly CONSOLE_I16_STR_DIGITS zero-padded characters into buffer, 
    // followed by a null terminator. Digits are extracted least-significant-first and
    // written directly into their final position, so no reversal pass is required.
    //
    // 'i' counts down from CONSOLE_I16_STR_DIGITS to 1, decremented in the loop's third
    // clause and used after subtracting 1 each time - so it only ever indexes buffer 
    // as [0, digits-1].
    for (uint8_t i = CONSOLE_I16_STR_DIGITS; i > 0; i--) {
        console_conversionBuffer[i] = (char)('0' + (uint8_t)(value % 10));
        value /= 10; 
    }

    console_conversionBuffer[CONSOLE_I16_STR_DIGITS + 1] = 0;
    console_printString(console_conversionBuffer);
}

void console_printHexByte(uint8_t value) {
    CONSOLE_PORT_PUTCHR(console_hexTable[(value >> 4)]);
    CONSOLE_PORT_PUTCHR(console_hexTable[value & 0x0F]);
    CONSOLE_PORT_FLUSH();
}

void console_printHexWord(uint16_t value) {
    console_printHexByte(value >> 8);
    console_printHexByte(value & 0xFF);
}
