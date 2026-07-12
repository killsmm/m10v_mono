------------------------------------------------------------------
Preview sample program

                          SOCIONEXT CONFIDENTIAL & Copyright 2018
------------------------------------------------------------------
[Overview]
 Display Preview.


[Example of command]
 cui_sample.sh preview


[Processing flow]
 1. IPCU initialization

 2. Boot-up

 3. Preview

 4. Image processing parameter change

 5. Release IPCU initialization

 6. Termination


[Detailed Description]
1. IPCU initialization
 Open mailbox for sending and receiving commands and
 receiving events and register each callback function.

2. Boot-up
 Send a command to notify the RTOS side that it has
 booted and notify it.

3. Preview
 Transmit mode change (IDLE -> Preview) command to
 RTOS side.

4. Image processing parameter change
 Wait for image processing parameter input from the console.
 Send command by input image processing parameter contents.

5. Release IPCU initialization
 Releases mailbox opening for command transmission and
 reception and event reception.
