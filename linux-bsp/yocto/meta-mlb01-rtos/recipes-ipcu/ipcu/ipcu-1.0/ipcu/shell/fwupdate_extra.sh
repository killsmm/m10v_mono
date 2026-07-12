#!/bin/sh
addr=0xA9343000
file=/media/root/mtp/DATA.bin
#file=/media/root/mtp/LINUX_2core.BIN
usage="
default mode is HID feedback
usage:
        sh /usr/bin/fwupdate_extra.sh 1

parameters:
        1   :       for CDC feedback
        "

update(){
	echo "start to update" > /dev/ttyGS0
	idx=0x16
#	idx=0x5

	#s=$(busybox wc -c < "$file")
	s=$(stat -c%s "$file")
	echo "file in byte:" $s
#	size=$(printf '0x%x\n' $(($s + 0x1000)))
	size=$(printf '0x%x\n' $(($s + 0)))
	echo "file in hex:" $size
	cd /usr/bin/
	./fwupdate -a $addr -s $size -f $file
	camera_if_direct 0xe 0x0 $idx
	camera_if_direct 0xe 0x1 $addr
	camera_if_direct 0xe 0x2 $size
	camera_if_direct 0xe 0x3 0x1 > /tmp.log
}

check_status(){
	echo "check_status if tmp.log exist or not"
	while true
	do
		if [ -f "/tmp.log" ]; then
			line=$(grep "0xe:0x3 successfully." /tmp.log)
			break;
		else
			sleep 1
		fi
	done
	#cat /tmp.log
	#echo "check_status 2"
	#line=$(grep "0xe:0x3 successfully." /tmp.log)
	#if [ $(echo "${line}" | busybox wc -l) -eq 1 ]; then
	#	echo "update successful, please restart 1"
	#else
	#	echo "update failed 1"
	#fi

	if [ "$line" = "0xe:0x3 successfully." ]; then
			rm -f /media/root/mtp/go_update.conf
			rm -f /media/root/mtp/DATA.bin
			echo "update successful, system will reboot after 1 seconds"
			echo "update successful, system will reboot after 1 seconds" > /dev/ttyGS0
			sleep 1
			camera_if_direct 0xd 0x1f 0x1
	else
			echo "update failed" > /dev/ttyGS0
			echo "update failed"
	fi
}
echo "waiting..."
sh /usr/bin/mtp_restart_hid.sh #stop hid then start mtp and restart hid.
sleep 1
while true
do

	#1. check if mtp is mounted
	if [ -d "/media/root/mtp" ]; then

		#echo "mtp is mounted"
		#echo "mtp is mounted" > /dev/ttyGS0
		
		#2. check if we need to update
		if [ -f "/media/root/mtp/go_update.conf" ]; then
			echo "need to update"
			#3. check DATA.bin
			if [ -f $file ]; then
	    	echo $file " exists, will go to update fw after 1 seconds"
	    	sleep 1
	    	#4. update
				update
				#5. check status
				echo "check_status after 3 seconds"
				#sleep 3
				if [ "$1" == "1" ] ; then
					echo "CDC mode"
				else
					echo "HID mode"
				fi
				check_status
				break;
    	fi
	  else
	  	sleep 1	
	  	#echo $file "doesn't exist"
	  #end of 2
		fi
	else
		sleep 1
		#echo "mtp not mount"
	#end of 1
	fi
#end of while
done
