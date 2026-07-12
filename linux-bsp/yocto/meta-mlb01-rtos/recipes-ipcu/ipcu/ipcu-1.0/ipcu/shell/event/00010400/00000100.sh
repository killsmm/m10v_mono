#!/bin/sh

echo "edid received" > /dev/ttyUSI0

fb_connector get_vic $1 $2 $3 $4

VIC=$?

if [ $VIC -eq 255 ]; then
	echo "error! not determine video size" > /dev/ttyUSI0
else
	# inform determine VIC to RTOS
	# camera_if s_hdmi_connection $VIC

	# get video size from VIC
	fb_connector get_vsize $VIC

	# setting to FB
	dtest hdmi-gr0 0x5004 $?
	dtest hdmi-gr0 0x5003 1
	dtest hdmi-gr0 0x4606 /tmp/test.txt 20 20 90
fi

echo $1 >> /tmp/edid_received.txt
echo $2 >> /tmp/edid_received.txt
echo $3 >> /tmp/edid_received.txt
echo $4 >> /tmp/edid_received.txt
echo  >> /tmp/edid_received.txt
