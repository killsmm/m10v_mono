#!/bin/bash


ret=1

mkdir -p /media/root/fw_upgrade
mount -t ipcufs mnt /media/root/fw_upgrade -ofs=1
update_file_path="/media/root/fw_upgrade/"
initramfs_file=`ls $update_file_path | grep initramfs`
rtos_file=`ls $update_file_path | grep rtos`
kernel_file=`ls $update_file_path | grep kernel`
data_file=`ls $update_file_path | grep DATA`

camera_if_direct 0x0 0xb 0x1


update_initramfs(){
	echo "start to flash..."
	file_size=$(printf "%x\n" `stat -c %s $update_file_path$initramfs_file`)
	echo file_size = $file_size
	fwupdate -a 6e5e0000 -s $file_size -f $update_file_path$initramfs_file
	camera_if_direct 0xe 0x0 0x6
	camera_if_direct 0xe 0x1 0x6e5e0000
	camera_if_direct 0xe 0x2 0x$file_size
	camera_if_direct 0xe 0x3 0x1
}

update_rtos(){
	echo "start to flash..."
	file_size=$(printf "%x\n" `stat -c %s $update_file_path$rtos_file`)
	echo file_size = $file_size
	fwupdate -a 6e5e0000 -s $file_size -f $update_file_path$rtos_file
	camera_if_direct 0xe 0x0 0x2
	camera_if_direct 0xe 0x1 0x6e5e0000
	camera_if_direct 0xe 0x2 0x$file_size
	camera_if_direct 0xe 0x3 0x1
}


update_kernel(){
	echo "start to flash..."
	file_size=$(printf "%x\n" `stat -c %s $update_file_path$kernel_file`)
	echo file_size = $file_size
	fwupdate -a 6e5e0000 -s $file_size -f $update_file_path$kernel_file
	camera_if_direct 0xe 0x0 0x5
	camera_if_direct 0xe 0x1 0x6e5e0000
	camera_if_direct 0xe 0x2 0x$file_size
	camera_if_direct 0xe 0x3 0x1
}

update_data(){
	echo "start to flash..."
	file_size=$(printf "%x\n" `stat -c %s $update_file_path$data_file`)
	echo file_size = $file_size
	fwupdate -a 6e5e0000 -s $file_size -f $update_file_path$data_file
	camera_if_direct 0xe 0x0 0x16
	camera_if_direct 0xe 0x1 0x6e5e0000
	camera_if_direct 0xe 0x2 0x$file_size
	camera_if_direct 0xe 0x3 0x1
}


if [[ -n "$initramfs_file"  &&  `cat /media/root/fw_upgrade/i.version` != "$initramfs_file" ]]
then 
	echo ">>start to update initramfs"
	var=(${initramfs_file//_/ }) #把"_"替换为空格，然后分割
	filename_part=${var[0]}
	filename_version=${var[1]}
	filename_checksum=${var[2]}
	checksum=(`md5sum $update_file_path$initramfs_file`)
	if [[ ${checksum[0]} == ${filename_checksum} && ${filename_part} == "initramfs" ]]
	then
		echo "checksum valid start to update..."
		update_initramfs
		if [ $? -ne 0 ]
		then
			exit 5
		fi
		ret=0
		echo "end update"
	else
		echo "checksum invalid stop update initramfs"
		exit 1
	fi
fi

if [[ -n "$rtos_file" &&  `cat /media/root/fw_upgrade/r.version` != "$rtos_file" ]]
then
	echo ">>start to update rtos"
	var=(${rtos_file//_/ }) #把"_"替换为空格，然后分割
	filename_part=${var[0]}
	filename_version=${var[1]}
	filename_checksum=${var[2]}
	checksum=(`md5sum $update_file_path$rtos_file`)
	if [[ ${checksum[0]} == ${filename_checksum} && ${filename_part} == "rtos" ]]
	then
		echo "checksum valid start to update..."
		update_rtos
		if [ $? -ne 0 ]
		then
			exit 5
		fi
		ret=0
		echo "end update"
	else
		echo "checksum invalid stop update rtos"
		exit 1
	fi
fi

if [[ -n "$kernel_file" && `cat /media/root/fw_upgrade/k.version` != "$kernel_file" ]]
then
	echo ">>start to update kernel"
	var=(${kernel_file//_/ }) #把"_"替换为空格，然后分割
	filename_part=${var[0]}
	filename_version=${var[1]}
	filename_checksum=${var[2]}
	checksum=(`md5sum $update_file_path$kernel_file`)
	if [[ ${checksum[0]} == ${filename_checksum} && ${filename_part} == "kernel" ]]
	then
		echo "checksum valid start to update..."
		update_kernel
		if [ $? -ne 0 ]
		then
			exit 5
		fi
		ret=0
		echo "end update"
	else
		echo "checksum invalid stop update kernel"
		exit 1
	fi
fi

if [ -n "$data_file" ]
then
	echo ">>start to update firmware"
	var=(${data_file//_/ }) #把"_"替换为空格，然后分割
	filename_part=${var[0]}
	filename_version=${var[1]}
	filename_checksum=${var[2]}
	checksum=(`md5sum $update_file_path$data_file`)
	if [[ ${checksum[0]} == ${filename_checksum} && ${filename_part} == "DATA" ]]
	then
		echo "checksum valid start to update..."
		update_data
		if [ $? -ne 0 ]
		then
			exit 5
		fi
		ret=0
		echo "end update"
	else
		echo "checksum invalid stop update data"
		exit 1
	fi
fi

echo "ret = ${ret}"

if [ ${ret} -eq 0 ]
then
	echo "$initramfs_file" > /media/root/fw_upgrade/i.version
	echo "$rtos_file" > /media/root/fw_upgrade/r.version
	echo "$kernel_file" > /media/root/fw_upgrade/k.version
	camera_if_direct 0x0 0x2 0x1
	exit 0
fi
exit ${ret}