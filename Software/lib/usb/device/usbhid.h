/********************************** (C) COPYRIGHT *******************************
* File Name     : USBHID.H
* Author        : Paul Raspa (PR77)
* License       : MIT
* Version       : V1.0
* Date          : 2026/02/01
* Description   : USB HID APIs
*******************************************************************************/

#ifndef __CH554_USB_HID_H__
#define __CH554_USB_HID_H__

#include <stdint.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>
#include "ch554.h"
#include "usb.h"
#include "usbconstant.h"
#include "usbhandler.h"

typedef void (*epOutHandler_t)(uint8_t length, __xdata uint8_t *report) __reentrant;

#define USBHID_MOUSE_REPORT_ID      1
#define USBHID_MOUSE_REPORT_SIZE    5

#define USBHID_CONSUMER_REPORT_ID   2
#define USBHID_CONSUMER_REPORT_SIZE 3

#define USBHID_KEYBOARD_REPORT_ID   3
#define USBHID_KEYBOARD_REPORT_SIZE 8

#define USBHID_LED_CTRL_REPORT_ID   4
#define USBHID_LED_CTRL_REPORT_SIZE 2

// Mouse Buttons
#define MOUSE_BUTTON_LEFT           0x01
#define MOUSE_BUTTON_RIGHT          0x02
#define MOUSE_BUTTON_MIDDLE         0x04

// Consumer Keyboard Keycodes
#define CON_SYS_POWER               0x30
#define CON_SYS_RESET               0x31
#define CON_SYS_SLEEP               0x32

#define CON_VOL_MUTE                0xE2
#define CON_VOL_UP                  0xE9
#define CON_VOL_DOWN                0xEA

#define CON_MEDIA_PLAY              0xB0
#define CON_MEDIA_PAUSE             0xB1
#define CON_MEDIA_RECORD            0xB2
#define CON_MEDIA_FORWARD           0xB3
#define CON_MEDIA_REWIND            0xB4
#define CON_MEDIA_NEXT              0xB5
#define CON_MEDIA_PREV              0xB6
#define CON_MEDIA_STOP              0xB7
#define CON_MEDIA_EJECT             0xB8
#define CON_MEDIA_RANDOM            0xB9

#define CON_MENU                    0x40
#define CON_MENU_PICK               0x41
#define CON_MENU_UP                 0x42
#define CON_MENU_DOWN               0x43
#define CON_MENU_LEFT               0x44
#define CON_MENU_RIGHT              0x45
#define CON_MENU_ESCAPE             0x46
#define CON_MENU_INCR               0x47
#define CON_MENU_DECR               0x48

#define KBD_KEY_NONE                0x00
#define KBD_KEY_ERR_OVF             0x01

// Modifier keys Bitmaks
#define KBD_KEY_LEFT_CTRL           0x01
#define KBD_KEY_LEFT_SHIFT          0x02
#define KBD_KEY_LEFT_ALT            0x04
#define KBD_KEY_LEFT_GUI            0x08
#define KBD_KEY_RIGHT_CTRL          0x10
#define KBD_KEY_RIGHT_SHIFT         0x20
#define KBD_KEY_RIGHT_ALT           0x40
#define KBD_KEY_RIGHT_GUI           0x80

#define KBD_KEY_A                   0x04 
#define KBD_KEY_B                   0x05 
#define KBD_KEY_C                   0x06 
#define KBD_KEY_D                   0x07 
#define KBD_KEY_E                   0x08 
#define KBD_KEY_F                   0x09 
#define KBD_KEY_G                   0x0A 
#define KBD_KEY_H                   0x0B 
#define KBD_KEY_I                   0x0C 
#define KBD_KEY_J                   0x0D 
#define KBD_KEY_K                   0x0E 
#define KBD_KEY_L                   0x0F 
#define KBD_KEY_M                   0x10 
#define KBD_KEY_N                   0x11 
#define KBD_KEY_O                   0x12 
#define KBD_KEY_P                   0x13 
#define KBD_KEY_Q                   0x14 
#define KBD_KEY_R                   0x15 
#define KBD_KEY_S                   0x16 
#define KBD_KEY_T                   0x17 
#define KBD_KEY_U                   0x18 
#define KBD_KEY_V                   0x19 
#define KBD_KEY_W                   0x1A 
#define KBD_KEY_X                   0x1B 
#define KBD_KEY_Y                   0x1C 
#define KBD_KEY_Z                   0x1D 

#define KBD_KEY_1                   0x1E 
#define KBD_KEY_2                   0x1F 
#define KBD_KEY_3                   0x20 
#define KBD_KEY_4                   0x21 
#define KBD_KEY_5                   0x22 
#define KBD_KEY_6                   0x23 
#define KBD_KEY_7                   0x24 
#define KBD_KEY_8                   0x25 
#define KBD_KEY_9                   0x26 
#define KBD_KEY_0                   0x27 

#define KBD_KEY_ENTER               0x28

// Special keys
#define KBD_KEY_UP_ARROW            0xDA
#define KBD_KEY_DOWN_ARROW          0xD9
#define KBD_KEY_LEFT_ARROW          0xD8
#define KBD_KEY_RIGHT_ARROW         0xD7
#define KBD_KEY_BACKSPACE           0xB2
#define KBD_KEY_TAB                 0xB3
#define KBD_KEY_RETURN              0xB0
#define KBD_KEY_ESC                 0xB1
#define KBD_KEY_INSERT              0xD1
#define KBD_KEY_DELETE              0xD4
#define KBD_KEY_PAGE_UP             0xD3
#define KBD_KEY_PAGE_DOWN           0xD6
#define KBD_KEY_HOME                0xD2
#define KBD_KEY_END                 0xD5
#define KBD_KEY_CAPS_LOCK           0xC1
#define KBD_KEY_F1                  0xC2
#define KBD_KEY_F2                  0xC3
#define KBD_KEY_F3                  0xC4
#define KBD_KEY_F4                  0xC5
#define KBD_KEY_F5                  0xC6
#define KBD_KEY_F6                  0xC7
#define KBD_KEY_F7                  0xC8
#define KBD_KEY_F8                  0xC9
#define KBD_KEY_F9                  0xCA
#define KBD_KEY_F10                 0xCB
#define KBD_KEY_F11                 0xCC
#define KBD_KEY_F12                 0xCD
#define KBD_KEY_F13                 0xF0
#define KBD_KEY_F14                 0xF1
#define KBD_KEY_F15                 0xF2
#define KBD_KEY_F16                 0xF3
#define KBD_KEY_F17                 0xF4
#define KBD_KEY_F18                 0xF5
#define KBD_KEY_F19                 0xF6
#define KBD_KEY_F20                 0xF7
#define KBD_KEY_F21                 0xF8
#define KBD_KEY_F22                 0xF9
#define KBD_KEY_F23                 0xFA
#define KBD_KEY_F24                 0xFB

void USB_EP1_IN(void);
void USB_EP1_RESET(void);
void USB_EP2_OUT(void);
void USB_EP2_RESET(void);

void usbhid_initialise(void);
void usbhid_deinitialise(void);
void usbhid_attachEPOutHandler(epOutHandler_t epOutHandlerFunction);
void usbhid_mouseWheelRotateUp(void);
void usbhid_mouseWheelRotateDown(void);
void usbhid_consumerMediaMute(void);
void usbhid_keyboardLockWorkstation(void);
void usbhib_nullHandler(void);

#endif // __CH554_USB_HID_H__