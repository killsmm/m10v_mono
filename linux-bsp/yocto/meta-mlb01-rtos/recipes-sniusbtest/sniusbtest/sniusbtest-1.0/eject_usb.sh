#!/bin/sh

if [ -e "/dev/mtp_usb" ]; then
   killall mtp-server
else
   echo '' > /sys/kernel/config/usb_gadget/g1/functions/mass_storage.usb0/lun.0/file
fi

