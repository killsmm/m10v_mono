#!/bin/sh

# UVC stop sequence
kill `ps | grep ip[c]uvc | awk '{print $1}'` >& /dev/null
kill `ps | grep estone-app | awk '{print $1}'` >& /dev/null
sleep 3
echo '' > /sys/kernel/config/usb_gadget/g1/UDC >& /dev/null
sleep 1
rm -r /sys/kernel/config/usb_gadget/g1/ >& /dev/null
rm -r /sys/kernel/config/usb_gadget/g1/ >& /dev/null
modprobe -r usb_f_uvc >& /dev/null
modprobe -r usb_f_hid >& /dev/null
modprobe -r libcomposite >& /dev/null
umount /sys/kernel/config/ >& /dev/null
modprobe -r configfs >& /dev/null
#modprobe -r vivid >& /dev/null
#modprobe -r font >& /dev/null
rmmod sn_usb20_udc.ko >& /dev/null