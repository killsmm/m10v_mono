#!/bin/sh
FW_UPDATE_TEMP_BUFF_ADDR=6e5e0000
#FW_UPDATE_TEMP_BUFF_ADDR=60000000

update_partition() { #$1 = file_name  $2 = partition_number
        echo "start to flash $1 to partition $2..."
        if [ ! -e "$1" ]; then
                echo "file $1 not exsit"
                exit 1
        fi
        camera_if_direct 0x0 0xb 0x1
        file_size=$(printf "%x\n" "$(stat -c %s "$1")")
        echo "file size = 0x$file_size"
        fwupdate -a "$FW_UPDATE_TEMP_BUFF_ADDR" -s "$file_size" -f "$1"
        camera_if_direct 0xe 0x0 "$2"
        camera_if_direct 0xe 0x1 "0x$FW_UPDATE_TEMP_BUFF_ADDR"
        camera_if_direct 0xe 0x2 0x"$file_size"
        camera_if_direct 0xe 0x3 0x1
}

print_help() {
        echo "please specify the part you want to update"
        echo "$0 kernel LINUX_2GB.BIN"
}

if [ $# -ne 2 ]; then
        print_help
        exit 1
fi

if [ "$1" = "rtos" ]; then
        update_partition $2 2
elif [ "$1" = "kernel" ]; then
        update_partition $2 5
elif [ "$1" = "initramfs" ]; then
        update_partition $2 6
elif [ "$1" = "resource" ]; then
        update_partition $2 8
elif [ "$1" = "data" ]; then
        update_partition $2 16

else
        print_help
fi
