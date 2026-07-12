------------------------------------------------------------------
Single Capture sample program

                          SOCIONEXT CONFIDENTIAL & Copyright 2018
------------------------------------------------------------------
[Overview]
 When the program is executed, the still image shooting data is
 saved on the SD card.


[Example of command]
 cui_sample.sh single_capture jpeg sd1 4000x3000 low


[Restrictions]
 This sample is not supported for operations other than this processing flow.


[Processing flow]
 1. single capture parameter setting

 2. IPCU initialization

 3. Boot-up

 4. Preview

 5. Single Capture

 6. Release IPCU initialization

 7. Termination


[About shooting still image shooting]
 This program does not support shutter key operation.
 Immediately perform still image shooting
 with "5. Single Capture".


[Detailed Description]
1. single capture parameter setting
 Check console input contents.
 The single capture parameter used when transmitting commands
 to the RTOS side is stored in the variable.

2. IPCU initialization
 Open mailbox for sending and receiving commands and
 receiving events and register each callback function.

3. Boot-up
 Send a command to notify the RTOS side that it has
 booted and notify it.

4. Preview
 Transmit mode change (IDLE -> Preview) command to RTOS side.

5. Single Capture
 Send command for parameter setting for Single capture to RTOS side.
 Send mode change (Preview-> Single capture) command to RTOS side.
 Wait for single capture completion event from RTOS side.
 Send mode change (Single capture-> Preview) command to RTOS side.

6. Release IPCU initialization
 Releases mailbox opening for command transmission and
 reception and event reception.

7. Termination
 Please turn off the power supply or reset the hardware.