#!/bin/sh
set -ex
insmod /lib/modules/4.4.15/kernel/drivers/usb/dwc3/dwc3-sn.ko hostmode=0
### User Setting
# Device descriptor Setting
# http://isticktoit.net/?p=1383
bcd_usb=0x0310		# USB 3.0
id_vendor=0x1d6b	# Linux Foundation
id_product=0x0104	# Multifunction Composite Gadget 
bcd_device=0x0090	# v 0.9
language_id=0x0409
sManufacture="Socionext Inc"
sProduct="SN-UVC"
sSerialnumber="01234567890123"

# Configuration descriptor Setting
sConfiguration="Config YUV"
bm_attributes=0xC0  # self power
streaming_interval=1
streaming_maxpacket=1024
streaming_maxburst=0
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
mkdir -p functions/uvc.usb0/streaming/uncompressed/u/360p
cat <<EOF > functions/uvc.usb0/streaming/uncompressed/u/360p/dwFrameInterval
666666
1000000
5000000
EOF
mkdir -p functions/uvc.usb0/streaming/header/h
cd functions/uvc.usb0/streaming/header/h
ln -s ../../uncompressed/u
cd ../../class/fs
ln -s ../../header/h
cd ../../class/hs
ln -s ../../header/h
cd ../../class/ss
ln -s ../../header/h
cd ../../../control
mkdir header/h
ln -s header/h class/fs
[ -e class/hs ] && ln -s header/h class/hs
[ -e class/ss ] && ln -s header/h class/ss
cd ../../../
echo $streaming_interval > functions/uvc.usb0/streaming_interval
echo $streaming_maxpacket > functions/uvc.usb0/streaming_maxpacket
echo $streaming_maxburst > functions/uvc.usb0/streaming_maxburst

# Create Configuration Instance
mkdir -p configs/c.1/strings/${language_id}
echo ${sConfiguration} > configs/c.1/strings/${language_id}/configuration
#echo ${bm_attributes} > configs/c.1/bmAttributes
echo ${max_power} > configs/c.1/MaxPower

# Bind Function Instance to Configuration Instance
ln -s functions/uvc.usb0 configs/c.1

# Available UDC drivers (for USB3.0)
ls /sys/class/udc | grep dwc3 > UDC

if [ -e /dev/video0 ]; then
  echo "Success"
  modprobe vivid
  echo "Run uvc-gadet to send video stream"
  uvc-gadget -u /dev/video0 -v /dev/video1
else
  echo "Failure: Failed to initialize UVC video output device."
fi
exit 0
