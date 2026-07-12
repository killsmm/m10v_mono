------------------------------------------------------------------
Video Capture Sample Program

                          SOCIONEXT CONFIDENTIAL & Copyright 2018
------------------------------------------------------------------
[Overview]
 When the program is executed, movie recording is performed
 for 10 seconds save the MP4 file on the SD card.


[Example of command]
 cui_sample.sh video_capture 3840x2160 60fps hevc


[Restrictions]
 This sample is not supported for operations other than this processing flow.


[Processing flow]
 1. Command line reading

 2. IPCU initialization

 3. Boot-up

 4. Preview

 5. Video Capture

 6. Release IPCU initialization

 7. Termination


[About recording start / stop]
 This program does not support shutter key operation.
 Immediately start recording with "5. Video Capture"
 Recording stops automatically after 10 seconds.


[Detailed Description]
1. Command line reading
 Check console input contents.
 The video parameter value is stored in the variable
 for storing the video parameter used when transmitting
 the command to the RTOS side.

2. IPCU initialization
 Open mailbox for command transmission and reception
 and event reception, Register each callback function.

3. Boot-up
 Send a command to notify the RTOS side that it has booted
 and notify it.

4. Preview
 Transmit mode change (IDLE -> Preview) command to RTOS side.

5. Video Capture
 Send mode change (Preview-> Video Capture) command to RTOS side.
 Wait for 10 seconds.
 Transmit mode change (Video Capture -> Preview)
 command to RTOS side.

6. Release IPCU initialization
 Releases mailbox opening for command transmission and
 reception and event reception.

7. Termination
 Please turn off the power supply or reset the hardware.
