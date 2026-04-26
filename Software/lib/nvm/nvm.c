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

void nvm_initialise(void) {

}

static void nvm_writeByte(uint8_t address, uint8_t data) {

    if (address >= DATA_FLASH_SIZE_MAX) {
        return;
    }

    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= bDATA_WE;
    SAFE_MOD = 0x00;

    // Data flash base address is DATA_FLASH_ADDR and writes are 16 bits. Only the lower
    // 8 bites contains the data to write so all writes must be on even addresses.
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    ROM_ADDR_L = address << 1;
    ROM_DATA_L = data;

    if ((ROM_STATUS & bROM_ADDR_OK) == bROM_ADDR_OK) {
        ROM_CTRL = ROM_CMD_WRITE;
    }

    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG &= ~bDATA_WE;
    SAFE_MOD = 0x00;
}

static uint8_t nvm_readByte(uint8_t address) {
    
    // Data flash base address is DATA_FLASH_ADDR and reads must be on even addresses.
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    ROM_ADDR_L = address << 1;
    
    ROM_CTRL = ROM_CMD_READ;
    return (ROM_DATA_L);
}

nvmStatus_e nvm_readBlock(uint8_t *data, uint8_t size) {

    (data);
    (size);

    return (nvmError);
}

nvmStatus_e nvm_writeBlock(uint8_t *data, uint8_t size) {

    (data);
    (size);

    return (nvmError);
}

