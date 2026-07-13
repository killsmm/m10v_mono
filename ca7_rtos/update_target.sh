#!/bin/sh
scp target/cpu_tkernel/project/bin/MILB_SDK.bin root@192.168.137.16:/home/root
ssh -l root 192.168.137.16 'update_seperately.sh rtos /home/root/MILB_SDK.bin'
if [ $? -eq 0 ];then
    echo "update successfully, reboot..."
    ssh -l root 192.168.137.16 'reboot'
else
    echo "update failed"
    exit 1
fi