------------------------------------------------------------------
Media Format sample program

                          SOCIONEXT CONFIDENTIAL & Copyright 2018
------------------------------------------------------------------
[Overview]
 When inserting the SD card into SD1 Card side and executing
 the program, Quick format the SD card.


[Example of command]
 cui_sample.sh media_format


[Restrictions]
 This sample is not supported for operations other than this processing flow.


[Processing flow]
 1. IPCU initialization

 2. Boot-up

 3. Media Format

 4. Release IPCU initialization

 5. Termination


[Detailed Description]
1. IPCU initialization
 Open mailbox for command transmission and reception and
 event reception, Register each callback function.

2. Boot-up
 Send a command to notify the RTOS side that it has booted and
 notify it.

3. Media Format
 Transmit Format command to RTOS side.

4. Release IPCU initialization
 Releases mailbox opening for command transmission and
 reception and event reception.

5. Termination
 Please turn off the power supply or reset the hardware.