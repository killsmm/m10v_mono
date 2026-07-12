#!/bin/sh
#insmod /lib/modules/4.4.15+/kernel/drivers/usb/dwc3/dwc3-sn.ko hostmode=1

if [ ! "$1" ];then
	echo "$0 <display set>"
	exit 1
fi

DIR="$(busybox dirname "$0")"
while true
do
	fb=$(ls /dev | grep fb0)
	if [ $fb ];then
		echo "got fb!"
		break
	else
		echo "did not get fb!"
		sleep 1s
	fi
done
#cp -r rootfs/usr/lib/fonts/ /usr/lib
#source enviorenment.sh
#dtest hdmi-gr0 0x5004 1
#dtest hdmi-gr0 0x5003 1
#dtest hdmi-gr0 0x5004 "$1" 
"$DIR"/qte-fthdmi -qws -display linuxfb:/dev/fb2:mmWidth470:mmHeight270 -nomouse &
sleep 3s
dtest hdmi-gr0 0x5003 1
