# CH554 AUX KEYBOARD INTERFACE
USB MACRO PAD (AUX Keyboard Interface) based on the WCH554 MCS51 based chip with USB Device + Host Interface. 

# Warning
This design has not been compliance tested and will not be. It may cause personal damage to you or others. I take no responsibility for this. I accept no responsibility for any damage to any equipment that results from the use of this design and its components. IT IS ENTIRELY AT YOUR OWN RISK!

# Overview
This project is essentially an open source macro pad which emulates a USB Keyboard, Mouse and Comsumer Device. Additionally, there is an OUT USB end point supported to allow for HOST communication back to the device. My target is to make this easily configurable so different functions can be added to the Rotary Encoder, Push Buttons and OLED display. I'm using this project as an opportunity to learn more about the WCH55x micro controller.

# Appearance Final PCB Version
PCB version has been routed and ready for manufacture. Gerbers are not yet published until I have checked the PCB for issues. Here is the usual 3D artwork of the design.

![PCB](/Images/USBAuxKeyboardPCBTHCTop.png)
![PCB](/Images/USBAuxKeyboardPCBTop.png)
![PCB](/Images/USBAuxKeyboardPCBBottom.png)

# Credits
I refacted a lot of the original libraries into various software components. However there were some repos which provided me with excellent information, tools and resources;
- https://github.com/wagiminator
- https://github.com/Blinkinlabs/ch554_sdcc
- https://github.com/HonghongLu/CH554
- https://github.com/pkourany/clickButton
