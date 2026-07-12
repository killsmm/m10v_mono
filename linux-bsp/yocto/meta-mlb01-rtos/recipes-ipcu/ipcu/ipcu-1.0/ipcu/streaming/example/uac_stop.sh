#!/bin/sh

# UAC stop sequence
kill `ps | grep ip[c]uac | awk '{print $1}'` >& /dev/null
sleep 1
echo '' > /sys/kernel/config/usb_gadget/g1/UDC
sleep 1
rm -r /sys/kernel/config/usb_gadget/g1/ >& /dev/null
modprobe -r usb_f_uac1_mic >& /dev/null
modprobe -r libcomposite >& /dev/null
umount /sys/kernel/config/ >& /dev/null
modprobe -r configfs >& /dev/null
modprobe -r snd-pcm >& /dev/null
modprobe -r snd-timer >& /dev/null
modprobe -r snd >& /dev/null
modprobe -r soundcore >& /dev/null
rmmod sn_usb20_udc.ko >& /dev/null
rmmod dwc3-sn.ko >& /dev/null
