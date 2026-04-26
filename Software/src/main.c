/********************************** (C) COPYRIGHT *******************************
* File Name     : MAIN.c
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2024/12/17
* Description   : CH554 AUX KEYBOARD INTERFACE (MACRO PAD OR3)
*******************************************************************************/

#include <stdint.h>
#include <compiler.h>
#include "ch554.h"
#include "firmware_info.h"
#include "heartbeat.h"
#include "system.h"
#include "tick.h"
#include "i2c.h"
#include "ssd1306.h"
#include "serial.h"
#include "bootloader.h"
#include "ws2812.h"
#include "rotary.h"
#include "clickbtn.h"
#include "device/usbhid.h"
#include "main_gfx.h"
#include "menu.h"

#undef CONSOLE_DEBUG_ENABLED
#define CONSOLE_MENU_ENABLED

#if defined(CONSOLE_DEBUG_ENABLED) && defined(CONSOLE_MENU_ENABLED)
#error Only CONSOLE_DEBUG_ENABLED or CONSOLE_MENU_ENABLED can be enabled. Please disable one of the configurations
#endif

#define LED_FLASH_RATE_MS           300
#define WELCOME_SCREEN_DELAY_MS     800
#define LOG(msg)                    {serial_printString(__FILE__); serial_printCharacter(' '); serial_printHexWord(__LINE__); serial_printCharacter(' '); serial_printString(msg); serial_printString("\n\r");}
#define NUMBER_OF_HOTKEY_HANDLERS   (sizeof(hotKeyHandlers) / sizeof(hotKeyHandler_s))
#define NUMBER_OF_HOTKEY_MAPS       (sizeof(hotKeyMap) / sizeof(hotKeyMap_s))

typedef enum {
    rotaryDialClockwise = 0,
    rotaryDialCounterClockwise,
    rotaryDialSwitchShort,
    rotaryDialSwitchLong,
    macroSwitch1Short,
    macroSwitch1Long,
    macroSwitch2Short,
    macroSwitch2Long,
    macroSwitch3Short,
    macroSwitch3Long,

    NUMBER_OF_PHYSICAL_HOTKEYS
} physicalHotKey_e;

static const char * const physicalHotKeyDescriptions[NUMBER_OF_PHYSICAL_HOTKEYS] = {
    {"rotaryDialClockwise"},
    {"rotaryDialCounterClockwise"},
    {"rotaryDialSwitchShort"},
    {"rotaryDialSwitchLong"},
    {"macroSwitch1Short"},
    {"macroSwitch1Long"},
    {"macroSwitch2Short"},
    {"macroSwitch2Long"},
    {"macroSwitch3Short"},
    {"macroSwitch3Long"},
};

#define PHYSICAL_HOTKEY_DESC_WIDTH  30

// TODO: DATA-Flash table to translate the physicalHotKey number (i.e., enum rotary
// encoder dial or switch, MACRO SW 1, 2 and 3, etc) to a mapped usbhid_xxx handler.
// The hotKeyMap would be saved in DATA-Flash and modifable.

typedef struct {
    physicalHotKey_e hotKey;
    uint8_t hotKeyHandlerIndex;     
} hotKeyMap_s;

static const hotKeyMap_s hotKeyMap[] = {
    {rotaryDialClockwise,                       0},
    {rotaryDialCounterClockwise,                1},
    {rotaryDialSwitchShort,                     5},
    {rotaryDialSwitchLong,                      6},
    {macroSwitch1Short,                         9},
    {macroSwitch1Long,                          9},
    {macroSwitch2Short,                         9},
    {macroSwitch2Long,                          9},
    {macroSwitch3Short,                         9},
    {macroSwitch3Long,                          9}         
};

typedef struct {
    uint8_t handlerIndex;
    char *handlerText;
    char *hotkeyLabel;
    uint8_t xPositionLabel;
    uint8_t yPositionLabel;
    void (*commandHandler)(void);
} hotKeyHandler_s;


static const hotKeyHandler_s hotKeyHandlers[] = {
    {0x00,  "usbhid_mouseWheelRotateUp",        NULL,       0, 0, usbhid_mouseWheelRotateUp},
    {0x01,  "usbhid_mouseWheelRotateDown",      NULL,       0, 0, usbhid_mouseWheelRotateDown},
    {0x02,  "usbhid_consumerMediaMute",         NULL,       0, 0, usbhid_consumerMediaMute},
    {0x03,  "usbhid_keyboardLockWorkstation",   NULL,       0, 0, usbhid_keyboardLockWorkstation},
    {0x04,  "usbhid_consumerMediaBrowser",      NULL,       0, 0, usbhid_consumerMediaBrowser},
    {0x05,  "usbhid_consumerMediaEditor",       NULL,       0, 0, usbhid_consumerMediaEditor},
    {0x06,  "usbhid_consumerMediaCalculator",   NULL,       0, 0, usbhid_consumerMediaCalculator},
    {0x07,  "usbhib_nullHandler",               "<<NOP>>",  4, 2, usbhib_nullHandler},
    {0x08,  "usbhib_nullHandler",               "<<NOP>>",  0, 3, usbhib_nullHandler},
    {0x09,  "usbhib_nullHandler",               "<<NOP>>",  8, 3, usbhib_nullHandler},
};

static void main_displayHotKeyMapping(void) {

    uint8_t mappedHotKeyHandlerIndex = 0;

    serial_printString("\nHotkey Mapping -----------------------------------------------------------------\n");

    // Itterate through all of the physical hotkeys
    for (uint8_t i = 0; i < NUMBER_OF_PHYSICAL_HOTKEYS; i++) {
        serial_printHexByte(i);
        serial_printString(":\t");

        if (i != (uint8_t)hotKeyMap[i].hotKey) {
            serial_printString("Mapping ERROR detected");
        } else {
            mappedHotKeyHandlerIndex = hotKeyMap[i].hotKeyHandlerIndex;
            serial_printStringPadded(physicalHotKeyDescriptions[i], PHYSICAL_HOTKEY_DESC_WIDTH);
            serial_printString(" --> ");

            if (NUMBER_OF_HOTKEY_HANDLERS > mappedHotKeyHandlerIndex) {
                serial_printString(hotKeyHandlers[mappedHotKeyHandlerIndex].handlerText);
            } 
        }

        serial_printCharacter('\n');
    }
}

static void main_triggerHotKeyHandler(physicalHotKey_e physicalHotKey) {

    uint8_t foundHotKeyHandlerIndex = 0;
    
    // TODO: DATA-Flash table to translate the physicalHotKey number (i.e., enum rotary
    // encoder dial or switch, MACRO SW 1, 2 and 3, etc) to a mapped usbhid_xxx handler.
    // The hotKeyMap would be saved in DATA-Flash and modifable.
   
    for (uint8_t i = 0; i < NUMBER_OF_PHYSICAL_HOTKEYS; i++) {
        if (physicalHotKey == hotKeyMap[i].hotKey) {
            foundHotKeyHandlerIndex = hotKeyMap[i].hotKeyHandlerIndex;

            // Quick sanity check to ensure hotKeyHandlerIndex in the mapping structure
            // actually matches handlerIndex in our support hotkey handlers.
            if (foundHotKeyHandlerIndex != hotKeyHandlers[foundHotKeyHandlerIndex].handlerIndex) {
                break;
            }

            if (hotKeyHandlers[foundHotKeyHandlerIndex].commandHandler != NULL) {
                hotKeyHandlers[foundHotKeyHandlerIndex].commandHandler();
            }
            break;
        }    
    }
}

static void main_epHandler(uint8_t length, __xdata uint8_t *report) __reentrant {
    
    // IMPORTANT: This function should _REALLY_ be treated like an ISR and it is essentially
    // called from usbhandler_USBInterrupt() which is the USB ISR handler. What I've done
    // here is _REALLY_ just for testing.

    if ((report[0] == USBHID_LED_CTRL_REPORT_ID) && (length == USBHID_LED_CTRL_REPORT_SIZE)) {
        uint8_t currentWS2812Colour = report[1];
    
        for (uint8_t i = 0; i < WS2812_PIXEL_COUNT; i++) {
            ws2812_setPixelWheelColour(i, currentWS2812Colour + (i * (UINT8_MAX / (WS2812_PIXEL_COUNT - 1))));
            ws2812_setPixelBrightess(i, brightness25Percent);
        }
        ws2812_updatePixels();
    }
}

void main(void) {
    __xdata rotaryDirection_e currentRotaryDirection = noRotation;
    __xdata uint32_t previousCountLEDFlash = 0;
    __xdata uint8_t currentWS2812Colour = 0, currentImageIndex = 0;

    // Setup low level system and bootloader
    system_disableGlobalInterupts();
    system_CfgFsys();
    bootloader_initialise();

    // Setup timer ticks
    tick_initialiseTimer0();
    tick_enableTimer0Interrupt();

    // Setup heartbeat LED
    heartbeat_initialise();

#if defined(CONSOLE_DEBUG_ENABLED) || defined (CONSOLE_MENU_ENABLED)
    // Setup serial port (debug)
    serial_initialiseSerial1(SERIAL_BAUD_RATE, 0);
#endif // CONSOLE_DEBUG_ENABLED or CONSOLE_MENU_ENABLED

    // Setup i2c and SSD1306 OLED
    i2c_initialise();
    ssd1306_initialise();
    ssd1306_clearScreen();

    // Setup rotary switch decoder and button debouncer
    rotary_initialise();
    clickbtn_initialise();

    // Start timer tick interruprt
    tick_startTimer0();
    tick_enableTimer0Interrupt();

    // USB HID Device initialisation
    usbhid_initialise();
    usbhid_attachEPOutHandler(main_epHandler);

    // Enable global interrupts
    system_enableGlobalInterupts();

    // Setup WS2812 LED Driver and initialise all pixels (OFF)
    ws2812_initialise();
    ws2812_displayFullWheelColour(brightness25Percent);

    serial_printString("\x1b[2J\x1b[H");
    serial_printString(FW_NAME);
    serial_printCharacter(' ');
    serial_printString(FW_VERSION);
    serial_printCharacter(' ');
    serial_printString(FW_DESC);
    serial_printString("\n\r");

#if defined(CONSOLE_MENU_ENABLED)
    menu_initialise();
#endif // CONSOLE_MENU_ENABLED

    main_displayHotKeyMapping();
    
    // Display welcome message and basic HMI elements
    ssd1306_drawBmp(96, 0, 32, 32, bmpImageList[0]);
    ssd1306_setCursor(0, 0);
    ssd1306_printString(FW_SLASH_1);
    ssd1306_setCursor(0, 1);
    ssd1306_printString(FW_SLASH_2);
    for (uint8_t i = 0; i < NUMBER_OF_HOTKEY_HANDLERS; i++) {
        if (hotKeyHandlers[i].hotkeyLabel != NULL) {
            ssd1306_setCursor(hotKeyHandlers[i].xPositionLabel, hotKeyHandlers[i].yPositionLabel);
            ssd1306_printString(hotKeyHandlers[i].hotkeyLabel);
        }
    } 

    // Cyclic loop application code starts here
    while (1) {
        
#if defined(CONSOLE_DEBUG_ENABLED)
        uint16_t characterToEcho = serial_getCharacter(0);

        if ((characterToEcho != RECEIVE_TIMEOUT) && (characterToEcho != RECEIVE_NO_DATA_AVAIL)) {
            serial_printCharacter((char)characterToEcho);    
        }
#endif // CONSOLE_DEBUG_ENABLED

#if defined(CONSOLE_MENU_ENABLED)
        uint16_t consoleCharacter = serial_getCharacter(0);

        if ((consoleCharacter != RECEIVE_TIMEOUT) && (consoleCharacter != RECEIVE_NO_DATA_AVAIL)) {
            serial_printCharacter((char)consoleCharacter);
            menu_cyclicHandler((char)consoleCharacter);
        }
#endif // CONSOLE_MENU_ENABLED

        if (bootloader_checkBootloaderRequest()) {
            ws2812_initialise();
            ws2812_updatePixels();
            usbhid_deinitialise();            
            ssd1306_clearScreen();
            ssd1306_setCursor(0, 0);
            ssd1306_printString("---- BOOT LOADER ----");
            bootloader_enter();
        }

        if ((tick_get1msTimerCount() - previousCountLEDFlash) > LED_FLASH_RATE_MS) {
            previousCountLEDFlash += LED_FLASH_RATE_MS;

            heartbeat_toggleState();
        }

        rotary_cyclicHanlder();
        clickbtn_cyclicHandler();

        currentRotaryDirection = rotary_getDirection();
        if (currentRotaryDirection != noRotation) {
            if (currentRotaryDirection == clockwiseRotation) {
                currentWS2812Colour += 16;
                main_triggerHotKeyHandler(rotaryDialClockwise);

                if (currentImageIndex >= (sizeof(bmpImageList) / sizeof (bmpImageList[0])) - 1) {
                    currentImageIndex = 0;    
                } else {
                    currentImageIndex++;    
                }
                ssd1306_drawBmp(96, 0, 32, 32, bmpImageList[currentImageIndex]);
            }

            if (currentRotaryDirection == counterClockwiseRotation) {
                currentWS2812Colour -= 16;
                main_triggerHotKeyHandler(rotaryDialCounterClockwise);

                if (currentImageIndex > 0) {
                    currentImageIndex--;
                } else {
                    currentImageIndex = (sizeof(bmpImageList) / sizeof (bmpImageList[0])) - 1;    
                }
                ssd1306_drawBmp(96, 0, 32, 32, bmpImageList[currentImageIndex]);
            }

            for (uint8_t i = 0; i < WS2812_PIXEL_COUNT; i++) {
                ws2812_setPixelWheelColour(i, currentWS2812Colour + (i * (UINT8_MAX / (WS2812_PIXEL_COUNT - 1))));
                ws2812_setPixelBrightess(i, brightness25Percent);
            }
            ws2812_updatePixels();
        }

        if (clickbtn_getButtonState(ROTARY_ENC_SW_INDEX)->clicks > 0) {
            main_triggerHotKeyHandler(rotaryDialSwitchShort);
        }
        
        if (clickbtn_getButtonState(ROTARY_ENC_SW_INDEX)->clicks < 0) {
            main_triggerHotKeyHandler(rotaryDialSwitchLong);
        }

        if (clickbtn_getButtonState(MACRO_1_SW_INDEX)->clicks > 0) {
            main_triggerHotKeyHandler(macroSwitch1Short);
        }

        if (clickbtn_getButtonState(MACRO_1_SW_INDEX)->clicks < 0) {
            main_triggerHotKeyHandler(macroSwitch1Long);
        }

        if (clickbtn_getButtonState(MACRO_2_SW_INDEX)->clicks > 0) {
            main_triggerHotKeyHandler(macroSwitch2Short);
        }

        if (clickbtn_getButtonState(MACRO_2_SW_INDEX)->clicks < 0) {
            main_triggerHotKeyHandler(macroSwitch2Long);
        }

        if (clickbtn_getButtonState(MACRO_2_SW_INDEX /*Change to MACRO_2_SW_INDEX in final PCB*/)->clicks > 0) {
            main_triggerHotKeyHandler(macroSwitch3Short);
        }

        if (clickbtn_getButtonState(MACRO_2_SW_INDEX /*Change to MACRO_2_SW_INDEX in final PCB*/)->clicks < 0) {
            main_triggerHotKeyHandler(macroSwitch3Long);
        }
    }
}
