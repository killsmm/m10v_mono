#!/bin/sh

# check card enable or not
cat /sys/devices/platform/1b020000.mmc/enable | grep "0" > /dev/null

if [ $? = 0 ] ; then
	# change idle mode. OK_PF does not need
	#camera_if chg_camera_mode 1 > /tmp/mode_result.txt

	# check chenge mode result
	#cat /tmp/mode_result.txt | grep "Return 1 = 0x00000000" > /dev/null

	#if [ $? = 0 ] ; then
		# change unmount mode
		#camera_if chg_camera_mode 12 > /tmp/mode_result.txt
		# for OK_PF
		camera_if_direct 0x8 0x10 0x1 > /tmp/mode_result.txt

		# check chenge mode result
		cat /tmp/mode_result.txt | grep "Return 1 = 0x00000000" > /dev/null

		if [ $? = 0 ] ; then
			rm /tmp/mode_result.txt
		else
			echo "change unmount mode error!" > /dev/ttyUSI0
		fi
		return $?
	#else
	#	echo "change idle mode error!" > /dev/ttyUSI0
	#fi
fi
