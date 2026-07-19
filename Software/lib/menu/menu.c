/********************************** (C) COPYRIGHT *******************************
* File Name     : MENU.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/04/08
* Description   : Tokenised Menu Handler
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ch554.h"
#include "menu.h"
#include "menu_cfg.h"
#include "menu_helper.h"

static __xdata uint8_t menu_commandBuffer[MENU_COMMAND_BUFFER_SIZE];
static __xdata uint8_t menu_commandBufferIndex;

void menu_initialise(void) {

    memset(&menu_commandBuffer, 0, sizeof(menu_commandBuffer));
    menu_commandBufferIndex = 0;
}

void menu_processCommand(void) {

    char *command = menu_commandBuffer, *argument = NULL;

    // Delimit command and argument. Only 1 space between command and argument
    // is supported to keep this simple.
    for (char *tempPtr = menu_commandBuffer; *tempPtr; tempPtr++) {
        if (*tempPtr == ' ') {
            // Space found after command, so delimit command and substitute the space
            // with NULL terminator.
            *tempPtr = 0;
            // Argument is the next string.
            argument = tempPtr + 1;
            break;
        }
    }

    for (uint8_t i = 0; i < menu_getNumberOfMenuEnteries(); i++) {
        if ((command[0] | 0x20) == menuEntries[i].commandName) {
            // Since commands are only 1 character, check the first character only.
            // Additionally, OR the command character (ASCII) with 0x20 to allow for
            // case to be ignored. Lowercase has a higher ASCII value (offset of 0x20).
            if (menuEntries[i].commandHandler != NULL) {
                menuEntries[i].commandHandler(argument);
            }
            break;
        }
    }

    menu_printCommandPrompt();
} 

void menu_cyclicHandler(char character) {
     if (character == '\r') {
        menu_commandBuffer[menu_commandBufferIndex] = 0;
        menu_processCommand();
        menu_commandBufferIndex = 0;
    } else if (character == '\b' || character == 0x7F) {
        if (menu_commandBufferIndex > 0) {
            menu_commandBufferIndex--;
            menu_commandBuffer[menu_commandBufferIndex] = 0;
        }
    }  else {
        if (menu_commandBufferIndex < (MENU_COMMAND_BUFFER_SIZE - 1)) {
            menu_commandBuffer[menu_commandBufferIndex] = character;
            menu_commandBufferIndex++;
        } else {
            menu_signalError();
            menu_commandBufferIndex = 0;
        }
    }
}