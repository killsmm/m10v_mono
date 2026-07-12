#!/bin/sh

echo "Firmware Update finish" > /dev/ttyUSI0
echo "Firmware Update finish" > /dev/pts/0

echo $1 >> /tmp/fw_update_finish.txt
echo  >> /tmp/fw_update_finish.txt
