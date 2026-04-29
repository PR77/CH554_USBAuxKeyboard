/********************************** (C) COPYRIGHT *******************************
* File Name     : NVM.C
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/04/26
* Description   : Non-volatile memory (Data flash) handler
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "nvm.h"

// TODO - Need to implement level wearing. Can simply use a valid flag appended
// to the data block.

void nvm_initialise(void) {

}

nvmStatus_e nvm_readBlock(uint8_t *data, uint8_t startAddress, uint8_t size) {

    nvmStatus_e result = nvmOk;
    uint8_t address = 0;

    if (data == NULL) {
        return (nvmGeneralError);
    }
    
    if ((startAddress + size) > DATA_FLASH_SIZE_MAX) {
        return (nvmSizeError);
    }

    // Data flash base address is DATA_FLASH_ADDR and reads must be on even addresses.
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    address = startAddress << 1;
    
    for (uint8_t i = 0; i < size; i++) {
        ROM_ADDR_L = address + (i << 1);
        ROM_CTRL = ROM_CMD_READ;

        if ((ROM_STATUS & bROM_CMD_ERR) == 0) {
	        data[i] = ROM_DATA_L;
        } else {
            result = nvmCommandError;
            break;    
        }
    }

    return (result);
}

nvmStatus_e nvm_writeBlock(const uint8_t *data, uint8_t startAddress, uint8_t size) {

    nvmStatus_e result = nvmOk;
    uint8_t address = 0;
    
    if ((startAddress + size) > DATA_FLASH_SIZE_MAX) {
        return (nvmSizeError);
    }

    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= bDATA_WE;
    SAFE_MOD = 0x00;

    // Data flash base address is DATA_FLASH_ADDR and writes are 16 bits. Only the lower
    // 8 bites contains the data to write so all writes must be on even addresses.
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    address = startAddress << 1;

    for (uint8_t i = 0; i < size; i++) {
        ROM_ADDR_L = address + (i << 1);
        ROM_DATA_L = data[i];

        // Checking bROM_ADDR_OK can be done before triggering the ROM_CMD_WRITE request.
        if ((ROM_STATUS & bROM_ADDR_OK) == bROM_ADDR_OK) {
            ROM_CTRL = ROM_CMD_WRITE;
        } 
        
        // Check ROM_STATUS afect writing has occurred - Core is stopped during writing.
        if ((ROM_STATUS & bROM_CMD_ERR) == bROM_CMD_ERR) {
            result = nvmCommandError;
            break;
        } 
        
        if ((ROM_STATUS & bROM_ADDR_OK) == 0) {
            result = nvmAddressError;
            break;    
        }
    }

    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG &= ~bDATA_WE;
    SAFE_MOD = 0x00;

    return (result);
}

