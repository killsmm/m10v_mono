#!/bin/sh

# CDC stop sequence
echo '' > /sys/kernel/config/usb_gadget/g1/UDC
rm -r /sys/kernel/config/usb_gadget/g1

