#!/bin/sh

streaming_maxpacket=1024
streaming_maxburst=0
usb_port=2
with_jpeg="yes"

while getopts p:b:-: opt
do
  case $opt in
    "p") streaming_maxpacket="$OPTARG" ;;
    "b") streaming_maxburst="$OPTARG" ;;
    "-")
         case "$OPTARG" in
           "usb2") usb_port=2 ;;
           "usb3") usb_port=3 ;;
           "with-jpeg") with_jpeg="yes" ;;
           "without-jpeg") with_jpeg="no" ;;
         esac
           ;;
  esac
done

[ ${streaming_maxpacket} -gt 3072 ] && exit 1
[ ${streaming_maxpacket} -lt 1 ] && exit 1
[ ${streaming_maxburst} -gt 15 ] && exit 1
[ ${streaming_maxburst} -lt 0 ] && exit 1

if [ ${usb_port} = 2 ] ; then
  product="SN-UVC2"
  udc_name="usb20dev"
  insmod /lib/modules/`uname -r`/kernel/drivers/usb/gadget/udc/sn_usb20_udc.ko
else
  product="SN-UVC3"
  udc_name="dwc3"
fi

modprobe libcomposite
mount -t configfs none /sys/kernel/config/
cd /sys/kernel/config/usb_gadget/
mkdir g1
cd g1
mkdir strings/0x0409

echo 0x1D6B > idVendor
echo 0x0104 > idProduct
echo 64 > bMaxPacketSize0
echo 239 > bDeviceClass
echo 2 > bDeviceSubClass
echo 1 > bDeviceProtocol
echo Socionext Inc > strings/0x0409/manufacturer
echo ${product} > strings/0x0409/product
echo 01234567890123 > strings/0x0409/serialnumber

mkdir configs/c.1

mkdir functions/uvc15.usb0

echo 1 > functions/uvc15.usb0/streaming_interval
echo ${streaming_maxpacket} > functions/uvc15.usb0/streaming_maxpacket
echo ${streaming_maxburst} > functions/uvc15.usb0/streaming_maxburst

mkdir functions/uvc15.usb0/streaming/h264/ss

mkdir functions/uvc15.usb0/streaming/h264/ss/UHD
echo 1 > functions/uvc15.usb0/streaming/h264/ss/UHD/bFrameIndex
echo 3840 > functions/uvc15.usb0/streaming/h264/ss/UHD/wWidth
echo 2160 > functions/uvc15.usb0/streaming/h264/ss/UHD/wHeight
echo 333333 > functions/uvc15.usb0/streaming/h264/ss/UHD/dwFrameInterval
echo 333333 > functions/uvc15.usb0/streaming/h264/ss/UHD/dwDefaultFrameInterval

mkdir functions/uvc15.usb0/streaming/h264/ss/FHD
echo 2 > functions/uvc15.usb0/streaming/h264/ss/FHD/bFrameIndex
echo 1920 > functions/uvc15.usb0/streaming/h264/ss/FHD/wWidth
echo 1080 > functions/uvc15.usb0/streaming/h264/ss/FHD/wHeight
echo 333333 > functions/uvc15.usb0/streaming/h264/ss/FHD/dwFrameInterval
echo 333333 > functions/uvc15.usb0/streaming/h264/ss/FHD/dwDefaultFrameInterval

mkdir functions/uvc15.usb0/streaming/h264/ss/HD
echo 3 > functions/uvc15.usb0/streaming/h264/ss/HD/bFrameIndex
echo 1280 > functions/uvc15.usb0/streaming/h264/ss/HD/wWidth
echo 720 > functions/uvc15.usb0/streaming/h264/ss/HD/wHeight
echo 333333 > functions/uvc15.usb0/streaming/h264/ss/HD/dwDefaultFrameInterval
echo 333333 > functions/uvc15.usb0/streaming/h264/ss/HD/dwFrameInterval

mkdir functions/uvc15.usb0/streaming/h264/ss/VGA
echo 4 > functions/uvc15.usb0/streaming/h264/ss/VGA/bFrameIndex
echo 640 > functions/uvc15.usb0/streaming/h264/ss/VGA/wWidth
echo 480 > functions/uvc15.usb0/streaming/h264/ss/VGA/wHeight
echo 166666 > functions/uvc15.usb0/streaming/h264/ss/VGA/dwDefaultFrameInterval
echo 166666 > functions/uvc15.usb0/streaming/h264/ss/VGA/dwFrameInterval

mkdir functions/uvc15.usb0/streaming/h264/hs

mkdir functions/uvc15.usb0/streaming/h264/hs/UHD
echo 1 > functions/uvc15.usb0/streaming/h264/hs/UHD/bFrameIndex
echo 3840 > functions/uvc15.usb0/streaming/h264/hs/UHD/wWidth
echo 2160 > functions/uvc15.usb0/streaming/h264/hs/UHD/wHeight
echo 333333 > functions/uvc15.usb0/streaming/h264/hs/UHD/dwFrameInterval
echo 333333 > functions/uvc15.usb0/streaming/h264/hs/UHD/dwDefaultFrameInterval

mkdir functions/uvc15.usb0/streaming/h264/hs/FHD
echo 2 > functions/uvc15.usb0/streaming/h264/hs/FHD/bFrameIndex
echo 1920 > functions/uvc15.usb0/streaming/h264/hs/FHD/wWidth
echo 1080 > functions/uvc15.usb0/streaming/h264/hs/FHD/wHeight
echo 333333 > functions/uvc15.usb0/streaming/h264/hs/FHD/dwFrameInterval
echo 333333 > functions/uvc15.usb0/streaming/h264/hs/FHD/dwDefaultFrameInterval

mkdir functions/uvc15.usb0/streaming/h264/hs/HD
echo 3 > functions/uvc15.usb0/streaming/h264/hs/HD/bFrameIndex
echo 1280 > functions/uvc15.usb0/streaming/h264/hs/HD/wWidth
echo 720 > functions/uvc15.usb0/streaming/h264/hs/HD/wHeight
echo 333333 > functions/uvc15.usb0/streaming/h264/hs/HD/dwDefaultFrameInterval
echo 333333 > functions/uvc15.usb0/streaming/h264/hs/HD/dwFrameInterval

mkdir functions/uvc15.usb0/streaming/h264/hs/VGA
echo 4 > functions/uvc15.usb0/streaming/h264/hs/VGA/bFrameIndex
echo 640 > functions/uvc15.usb0/streaming/h264/hs/VGA/wWidth
echo 480 > functions/uvc15.usb0/streaming/h264/hs/VGA/wHeight
echo 166666 > functions/uvc15.usb0/streaming/h264/hs/VGA/dwDefaultFrameInterval
echo 166666 > functions/uvc15.usb0/streaming/h264/hs/VGA/dwFrameInterval

mkdir functions/uvc15.usb0/streaming/header/ss
mkdir functions/uvc15.usb0/streaming/header/hs
mkdir functions/uvc15.usb0/streaming/header/fs

ln -s functions/uvc15.usb0/streaming/h264/ss functions/uvc15.usb0/streaming/header/ss
ln -s functions/uvc15.usb0/streaming/header/ss functions/uvc15.usb0/streaming/class/ss
ln -s functions/uvc15.usb0/streaming/h264/hs functions/uvc15.usb0/streaming/header/hs
ln -s functions/uvc15.usb0/streaming/header/hs functions/uvc15.usb0/streaming/class/hs
ln -s functions/uvc15.usb0/streaming/h264/hs functions/uvc15.usb0/streaming/header/fs
ln -s functions/uvc15.usb0/streaming/header/fs functions/uvc15.usb0/streaming/class/fs

mkdir functions/uvc15.usb0/control/header/h
ln -s functions/uvc15.usb0/control/header/h functions/uvc15.usb0/control/class/fs
ln -s functions/uvc15.usb0/control/header/h functions/uvc15.usb0/control/class/ss

ln -s functions/uvc15.usb0 configs/c.1

if [ ${with_jpeg} = "yes" ] ; then

mkdir functions/uvc15s.usb0

echo 1 > functions/uvc15s.usb0/streaming_interval
echo ${streaming_maxpacket} > functions/uvc15s.usb0/streaming_maxpacket
echo ${streaming_maxburst} > functions/uvc15s.usb0/streaming_maxburst

mkdir functions/uvc15s.usb0/streaming/mjpeg/ss

mkdir functions/uvc15s.usb0/streaming/mjpeg/ss/UHD
echo 1 > functions/uvc15s.usb0/streaming/mjpeg/ss/UHD/bFrameIndex
echo 3840 > functions/uvc15s.usb0/streaming/mjpeg/ss/UHD/wWidth
echo 2160 > functions/uvc15s.usb0/streaming/mjpeg/ss/UHD/wHeight
echo 10000000 > functions/uvc15s.usb0/streaming/mjpeg/ss/UHD/dwDefaultFrameInterval
echo 10000000 > functions/uvc15s.usb0/streaming/mjpeg/ss/UHD/dwFrameInterval

mkdir functions/uvc15s.usb0/streaming/mjpeg/hs
mkdir functions/uvc15s.usb0/streaming/mjpeg/hs/UHD
echo 1 > functions/uvc15s.usb0/streaming/mjpeg/hs/UHD/bFrameIndex
echo 3840 > functions/uvc15s.usb0/streaming/mjpeg/hs/UHD/wWidth
echo 2160 > functions/uvc15s.usb0/streaming/mjpeg/hs/UHD/wHeight
echo 10000000 > functions/uvc15s.usb0/streaming/mjpeg/hs/UHD/dwDefaultFrameInterval
echo 10000000 > functions/uvc15s.usb0/streaming/mjpeg/hs/UHD/dwFrameInterval

mkdir functions/uvc15s.usb0/streaming/header/ss
mkdir functions/uvc15s.usb0/streaming/header/hs
mkdir functions/uvc15s.usb0/streaming/header/fs

ln -s functions/uvc15s.usb0/streaming/mjpeg/ss functions/uvc15s.usb0/streaming/header/ss
ln -s functions/uvc15s.usb0/streaming/header/ss functions/uvc15s.usb0/streaming/class/ss
ln -s functions/uvc15s.usb0/streaming/mjpeg/hs functions/uvc15s.usb0/streaming/header/hs
ln -s functions/uvc15s.usb0/streaming/header/hs functions/uvc15s.usb0/streaming/class/hs
ln -s functions/uvc15s.usb0/streaming/mjpeg/hs functions/uvc15s.usb0/streaming/header/fs
ln -s functions/uvc15s.usb0/streaming/header/fs functions/uvc15s.usb0/streaming/class/fs

mkdir functions/uvc15s.usb0/control/header/h
ln -s functions/uvc15s.usb0/control/header/h functions/uvc15s.usb0/control/class/fs
ln -s functions/uvc15s.usb0/control/header/h functions/uvc15s.usb0/control/class/ss

ln -s functions/uvc15s.usb0 configs/c.1

fi

ls /sys/class/udc | grep ${udc_name} > UDC

sleep 1

/usr/bin/ipcuvc15-ift -p ${streaming_maxpacket} -b ${streaming_maxburst} -j ${with_jpeg} &

exit 0
