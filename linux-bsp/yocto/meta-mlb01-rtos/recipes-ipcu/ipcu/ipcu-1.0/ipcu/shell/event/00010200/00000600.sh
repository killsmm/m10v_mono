#!/bin/sh
filename=`date +img%y%m%d%H%M%s.jpg`
external_sd=""
sd_size=`df | grep mmcblk0p1 | sed 's/  */ /g' | cut -d " " -f 2 | awk '{print int($0)}'`
echo "mmcblk0p1 size is ${sd_size}"
if [ ${sd_size} -gt 200000 ];then
	echo "mmcblk0p1 is external sd"
	external_sd="mmcblk0p1"
else
	sd_size=`df | grep mmcblk1p1  | sed 's/  */ /g' | cut -d " " -f 2 | awk '{print int($0)}'`
	echo "mmcblk1p1 size is ${sd_size}"
	if [ ${sd_size} -gt 200000 ];then
		echo "mmcblk1p1 is external sd"
		external_sd="mmcblk1p1"
	fi
fi

if [ "$external_sd" == "" ];then
	echo "no external sd, abandon picture"
	exit 1
fi
 
jpeg_receive $1 $2 "/run/SD/$external_sd/DCIM/$filename"
