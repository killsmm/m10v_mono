#!/bin/sh

# check card enable or not
cat /sys/devices/platform/1b020000.mmc/enable | grep "0" > /dev/null

if [ $? = 0 ] ; then
	# change SD control RTOS to Linux
	echo 1 > /sys/devices/platform/1b020000.mmc/enable

	sleep 1s
fi

# get device name new SD card
s3=$(ls /sys/devices/platform/1b020000.mmc/mmc_host/mmc1/mmc1\:*/block/) > /dev/null

if [ $? = 0 ] ; then
	echo "#!/bin/sh" > /tmp/usb_connect
	echo "" >> /tmp/usb_connect

	# connect MSC
	s1="echo "
	s2="/dev/"
	s4=" > /sys/kernel/config/usb_gadget/g1/functions/mass_storage.usb0/lun.0/file"
	echo ${s1}${s2}${s3}${s4} >> /tmp/usb_connect
	chmod 755 /tmp/usb_connect
	mv /tmp/usb_connect /tmp/usb_connect.sh
	sh /tmp/usb_connect.sh
fi
