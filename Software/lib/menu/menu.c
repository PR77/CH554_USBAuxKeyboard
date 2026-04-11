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
#include "ch554.h"
#include "menu.h"
#include "menu_cfg.h"
#include "menu_helper.h"

static __xdata uint8_t commandBuffer[MENU_COMMAND_BUFFER_SIZE];
static __xdata uint8_t commandBufferIndex;

void menu_initialise(void) {

    memset(&commandBuffer, 0, sizeof(commandBuffer));
    commandBufferIndex = 0;
}

void menu_processCommand(void) {

    char *command = 0, *argument = 0;

    command = strtok(commandBuffer, " \r\n");

    if (command != NULL) {
        argument = strtok(NULL, " \r\n");
        
        for (uint8_t i = 0; i < NUMBER_OF_MENU_ENTERIES; i++) {
            if (strcmp(command, menuEntries[i].commandName) == 0) {
                if (menuEntries[i].commandHandler != NULL) {
                    menuEntries[i].commandHandler(argument);
                }
            }
        }
    }

    menu_printCommandPrompt();
} 

void menu_cyclicHandler(char character) {

    if ((character == '\n') || (character == '\r')) {
        commandBuffer[commandBufferIndex] = 0;
        menu_processCommand();
        commandBufferIndex = 0;
    } else {
        if (commandBufferIndex < (MENU_COMMAND_BUFFER_SIZE - 1)) {
            commandBuffer[commandBufferIndex] = character;
            commandBufferIndex++;
        } else {
            menu_signalError();
        }
    }
}