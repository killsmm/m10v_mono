#!/bin/sh
addr=0xa9343000
offset=0
file_prefix=/run/SD/mmcblk0p1/

usage="
usage:
        sh /usr/bin/fwupdate.sh 1

parameters:
        1   :       for LINUX.BIN update
        2   :       for initramfs update
        3   :       for DATA.bin update	
        
        default file path is /run/SD/mmcblk0p1/, please modify file_prefix to change the path
        "

if [ -z "$1" ]; then
        echo "$usage"
        exit 1
fi

for var in "$@"
do
	if [ $var = 1 ] ; then
  		echo "linux"
  		file=$file_prefix"LINUX_2core.BIN"
  		idx=0x5
  		offset=0x1000
	elif [ $var = 2 ] ; then
  		echo "initramfs"
  		file=$file_prefix"initramfs_2core"
  		idx=0x6
  		offset=0x1000
	elif [ $var = 3 ] ; then
  		echo "data.bin"
  		file=$file_prefix"DATA.bin"
  		idx=0x16
	else
		echo "error"
		return
	fi

	echo "file:" $file

	s=$(stat -c%s "$file")
	#size=$(printf '0x%x\n' $(($s + 0x1000)))
	size=$(printf '0x%x\n' $(($s + $offset)))
	echo "file in hex:" $size
	/usr/bin/fwupdate -a $addr -s $size -f $file
	camera_if_direct 0xe 0x0 $idx
	camera_if_direct 0xe 0x1 $addr
	camera_if_direct 0xe 0x2 $size
	camera_if_direct 0xe 0x3 0x1

done

