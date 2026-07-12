#!/bin/sh

# MSC stop sequence
echo "" > /sys/kernel/config/usb_gadget/g1/functions/mass_storage.usb0/lun.0/file
echo '' > /sys/kernel/config/usb_gadget/g1/UDC
rm -r /sys/kernel/config/usb_gadget/g1

sleep 1s

# check card enable or not
cat /sys/devices/platform/1b020000.mmc/enable | grep "1" > /dev/null

if [ $? = 0 ] ; then
	# change SD control RTOS to Linux
	echo 0 > /sys/devices/platform/1b020000.mmc/enable

	#camera_if chg_camera_mode 1
	#For OK_PF
	camera_if_direct 0x8 0x10 0x0
fi

