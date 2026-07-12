#!/bin/sh

# MTP stop sequence
busybox killall mtp-server
echo '' > /sys/kernel/config/usb_gadget/mtp/UDC
rm -r /sys/kernel/config/usb_gadget/mtp
rmmod usb_f_mtp
rmmod libcomposite

