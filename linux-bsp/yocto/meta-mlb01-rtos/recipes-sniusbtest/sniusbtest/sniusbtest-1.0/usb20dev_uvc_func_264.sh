#!/bin/sh
set -ex

### User Setting
# Device descriptor Setting
# http://isticktoit.net/?p=1383
bcd_usb=0x0200		# USB 2.0
id_vendor=0x1d6b	# Linux Foundation
id_product=0x0104	# Multifunction Composite Gadget 
bcd_device=0x0110       # v 0.11
language_id=0x0409
sManufacture="Socionext Inc"
sProduct="SN-UVC"
sSerialnumber="01234567890123"
width=640
height=480

# Configuration descriptor Setting
sConfiguration="Config YUV"
bm_attributes=0xC0  # self power
streaming_interval=1
streaming_maxpacket=1024
max_power=120

# function Setting

# Mounting USB Gadget ConfigFS
modprobe libcomposite
if [ ! -d /sys/kernel/config/usb_gadget ] ; then
mount -t configfs none /sys/kernel/config/
fi
cd /sys/kernel/config/usb_gadget/
mkdir -p g1
cd g1

# Create Gadget Device Instance
echo ${bcd_usb} > bcdUSB
echo ${id_vendor} > idVendor
echo ${id_product} > idProduct
echo ${bcd_device} > bcdDevice 
mkdir -p strings/${language_id}
echo ${sManufacture} > strings/${language_id}/manufacturer 
echo ${sProduct} > strings/${language_id}/product 
echo ${sSerialnumber} > strings/${language_id}/serialnumber

# Create Function Instance
mkdir -p functions/uvc.usb0/streaming/h264/u/480p
echo ${width} > functions/uvc.usb0/streaming/h264/u/480p/wWidth
echo ${height} > functions/uvc.usb0/streaming/h264/u/480p/wHeight
echo 1000000 > functions/uvc.usb0/streaming/h264/u/480p/dwFrameInterval
echo 1000000 > functions/uvc.usb0/streaming/h264/u/480p/dwDefaultFrameInterval
mkdir -p functions/uvc.usb0/streaming/header/h
cd functions/uvc.usb0/streaming/header/h
ln -s ../../h264/u
cd ../../class/fs
ln -s ../../header/h
cd ../../class/hs
ln -s ../../header/h
cd ../../../control
mkdir header/h
echo 300000000 > header/h/dwClockFrequency
ln -s header/h class/fs
[ -e class/hs ] && ln -sf header/h class/hs
cd ../../../
echo $streaming_interval > functions/uvc.usb0/streaming_interval
echo $streaming_maxpacket > functions/uvc.usb0/streaming_maxpacket

# Create Configuration Instance
mkdir -p configs/c.1/strings/${language_id}
echo ${sConfiguration} > configs/c.1/strings/${language_id}/configuration
#echo ${bm_attributes} > configs/c.1/bmAttributes
echo ${max_power} > configs/c.1/MaxPower

# Bind Function Instance to Configuration Instance
ln -s functions/uvc.usb0 configs/c.1

# Available UDC drivers (for USB2.0)
ls /sys/class/udc | grep usb > UDC

if [ -e /dev/video0 ]; then
  echo "Success"
  uvc-gadget264 -a -d -i /home/root/usb_test/h264 -o 0 -r 0 -s 1 -f 2 -u /dev/video0
else
  echo "Failure: Failed to initialize UVC video output device."
fi
exit
