#!/bin/bash

# UPDATE_FILE_PATH="/media/SD"

usage(){
	echo -e "usage: $0 [update_file] \nexample: $0 123123_update.tar"
}

update_file_tar_full_path=$(readlink -f "$1")
update_file_tar=$(busybox basename "$update_file_tar_full_path")

if [ -z "$update_file_tar_full_path" ];then
	usage
	exit 1
fi

var=(${update_file_tar//_/ })
file_checksum=${var[0]}
checksum=($(busybox md5sum "$update_file_tar_full_path"))
echo "$file_checksum"
echo ${checksum[0]}
if [[ ${checksum[0]} != "$file_checksum" ]];then
	echo "file destroied checksum error!"
	exit 1
fi


if [ -n "$update_file_tar_full_path" ];then
	echo ".tar file found $update_file_tar_full_path"
	mkdir -p /tmp/update
	cd /tmp/update && tar xf "$update_file_tar_full_path"
	for file in /tmp/update/*
	do
		echo "$file"
		if [[ $file =~ "rtos" ]];then
			update_seperately.sh rtos "$file"
		elif [[ $file =~ "initramfs" ]];then
			update_seperately.sh initramfs "$file"
		elif [[ $file =~ "resource" ]];then
			update_seperately.sh resource "$file"
		elif [[ $file =~ "kernel" ]];then
			update_seperately.sh kernel "$file"
		fi
	done
else
	echo "no .tar file found"
fi