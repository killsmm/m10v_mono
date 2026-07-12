#!/bin/sh

echo "hdmi status changed" > /dev/ttyUSI0
if [ $2 -eq 1920 ]; then
	dtest hdmi-gr0 0x5004 2
elif [ $2 -eq 3840 ]; then
	dtest hdmi-gr0 0x5004 5
fi
echo $1 >> /tmp/hdmi_status.txt
echo  >> /tmp/hdmi_status.txt
