#!/bin/sh
insmod /lib/modules/4.4.15/kernel/drivers/usb/gadget/udc/sn_usb20_udc.ko
set -ex

setfmt=mjpeg
#setfmt=h264
### User Setting
# Device descriptor Setting
# http://isticktoit.net/?p=1383
bcd_usb=0x0200		# USB 2.0
id_vendor=0x1d6b	# Linux Foundation
id_product=0x0104	# Multifunction Composite Gadget 
bcd_device=0x0011       # v 0.11
language_id=0x0409
sManufacture="Socionext Inc"
sProduct="SN-UVC"
sSerialnumber="01234567890123"
#width=432
#height=240
width=1280
height=720

# Configuration descriptor Setting
sConfiguration="Config" 
bm_attributes=0xC0  # self power
streaming_interval=1
streaming_maxpacket=1024
max_power=500

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
echo 64 > bMaxPacketSize0
#echo 239 > bDeviceClass
#echo 2 > bDeviceSubClass
#echo 1 > bDeviceProtocol
mkdir -p strings/${language_id}
echo ${sManufacture} > strings/${language_id}/manufacturer 
echo ${sProduct} > strings/${language_id}/product 
echo ${sSerialnumber} > strings/${language_id}/serialnumber

# Create Function Instance
mkdir -p functions/uvc.usb0/streaming/$setfmt/u/480p
#echo 0xff > functions/uvc.usb0/streaming/$setfmt/u/bmaControls
#echo ${width} > functions/uvc.usb0/streaming/$setfmt/u/480p/wWidth
#echo ${height} > functions/uvc.usb0/streaming/$setfmt/u/480p/wHeight
echo 1 > functions/uvc.usb0/streaming/$setfmt/u/480p/bFrameIndex
echo 1000000 > functions/uvc.usb0/streaming/$setfmt/u/480p/dwFrameInterval
echo 1000000 > functions/uvc.usb0/streaming/$setfmt/u/480p/dwDefaultFrameInterval
#######
mkdir -p functions/uvc.usb0/streaming/$setfmt/u/720p
echo 2 > functions/uvc.usb0/streaming/$setfmt/u/720p/bFrameIndex
echo 720 > functions/uvc.usb0/streaming/$setfmt/u/720p/wWidth
echo 540 > functions/uvc.usb0/streaming/$setfmt/u/720p/wHeight
echo 1000000 > functions/uvc.usb0/streaming/$setfmt/u/720p/dwFrameInterval
echo 1000000 > functions/uvc.usb0/streaming/$setfmt/u/720p/dwDefaultFrameInterval
#######
mkdir -p functions/uvc.usb0/streaming/$setfmt/u/1080p
echo 3 > functions/uvc.usb0/streaming/$setfmt/u/1080p/bFrameIndex
echo 1080 > functions/uvc.usb0/streaming/$setfmt/u/1080p/wWidth
echo 720 > functions/uvc.usb0/streaming/$setfmt/u/1080p/wHeight
echo 1000000 > functions/uvc.usb0/streaming/$setfmt/u/1080p/dwFrameInterval
echo 1000000 > functions/uvc.usb0/streaming/$setfmt/u/1080p/dwDefaultFrameInterval
#######

mkdir -p functions/uvc.usb0/streaming/header/h
cd functions/uvc.usb0/streaming/header/h
ln -s ../../$setfmt/u
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
#echo ${sConfiguration} > configs/c.1/strings/${language_id}/configuration
#echo ${bm_attributes} > configs/c.1/bmAttributes
echo ${max_power} > configs/c.1/MaxPower

# Bind Function Instance to Configuration Instance
ln -s functions/uvc.usb0 configs/c.1

# Available UDC drivers (for USB2.0)
ls /sys/class/udc | grep usb > UDC

if [ -e /dev/video0 ]; then
  echo "Success"
  if [ $setfmt == "h264" ]; then
    LD_LIBRARY_PATH=/usr/lib /usr/bin/ipcuvc -f 2 -u /dev/video0 -s ${width}x${height} &
  else
    LD_LIBRARY_PATH=/usr/lib /usr/bin/ipcuvc -f 1 -u /dev/video0 -s ${width}x${height} &
  fi
else
  echo "Failure: Failed to initialize UVC video output device."
fi
exit 0
