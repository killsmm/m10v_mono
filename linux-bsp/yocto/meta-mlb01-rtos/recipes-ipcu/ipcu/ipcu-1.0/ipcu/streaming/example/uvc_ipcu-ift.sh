#!/bin/sh

streaming_maxpacket=1024
streaming_maxburst=0

while getopts p:b: opt
do
  case $opt in
    "p") streaming_maxpacket="$OPTARG" ;;
    "b") streaming_maxburst="$OPTARG" ;;
  esac
done

[ ${streaming_maxpacket} -gt 3072 ] && exit 1
[ ${streaming_maxpacket} -lt 1 ] && exit 1
[ ${streaming_maxburst} -gt 15 ] && exit 1
[ ${streaming_maxburst} -lt 0 ] && exit 1

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
echo SN-UVC3 > strings/0x0409/product
echo 01234567890123 > strings/0x0409/serialnumber

mkdir functions/uvc.usb0

echo 1 > functions/uvc.usb0/streaming_interval
echo ${streaming_maxpacket} > functions/uvc.usb0/streaming_maxpacket
echo ${streaming_maxburst} > functions/uvc.usb0/streaming_maxburst

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD
echo 1 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD
echo 2 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/wHeight
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD
echo 3 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/wHeight
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QUXGA
echo 4 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QUXGA/bFrameIndex
echo 3200 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QUXGA/wWidth
echo 2400 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QUXGA/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QUXGA/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QUXGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UXGA
echo 5 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UXGA/bFrameIndex
echo 1600 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UXGA/wWidth
echo 1200 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UXGA/wHeight
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UXGA/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UXGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/XGA
echo 6 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/XGA/bFrameIndex
echo 1024 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/XGA/wWidth
echo 768 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/XGA/wHeight
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/XGA/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/XGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA
echo 7 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/wHeight
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD
echo 1 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/wHeight
echo 2500000 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/dwFrameInterval
echo 2500000 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD
echo 2 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/wHeight
echo 666666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD
echo 3 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QUXGA
echo 4 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QUXGA/bFrameIndex
echo 3200 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QUXGA/wWidth
echo 2400 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QUXGA/wHeight
echo 2500000 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QUXGA/dwFrameInterval
echo 2500000 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QUXGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UXGA
echo 5 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UXGA/bFrameIndex
echo 1600 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UXGA/wWidth
echo 1200 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UXGA/wHeight
echo 666666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UXGA/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UXGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/XGA
echo 6 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/XGA/bFrameIndex
echo 1024 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/XGA/wWidth
echo 768 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/XGA/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/XGA/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/XGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA
echo 7 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/UHD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/UHD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/UHD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD
echo 2 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD
echo 3 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wHeight
echo 333333 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/QUXGA
echo 4 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/QUXGA/bFrameIndex
echo 3200 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/QUXGA/wWidth
echo 2400 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/QUXGA/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/QUXGA/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/QUXGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/UXGA
echo 5 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/UXGA/bFrameIndex
echo 1600 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/UXGA/wWidth
echo 1200 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/UXGA/wHeight
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/UXGA/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/UXGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/XGA
echo 6 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/XGA/bFrameIndex
echo 1024 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/XGA/wWidth
echo 768 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/XGA/wHeight
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/XGA/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/XGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA
echo 7 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/wHeight
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/UHD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/UHD/wHeight
echo 5000000 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/UHD/dwFrameInterval
echo 5000000 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/FHD
echo 2 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/FHD/wHeight
echo 2000000 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/FHD/dwFrameInterval
echo 2000000 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD
echo 3 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/QUXGA
echo 4 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/QUXGA/bFrameIndex
echo 3200 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/QUXGA/wWidth
echo 2400 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/QUXGA/wHeight
echo 5000000 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/QUXGA/dwFrameInterval
echo 5000000 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/QUXGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/UXGA
echo 5 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/UXGA/bFrameIndex
echo 1600 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/UXGA/wWidth
echo 1200 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/UXGA/wHeight
echo 2000000 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/UXGA/dwFrameInterval
echo 2000000 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/UXGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/XGA
echo 6 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/XGA/bFrameIndex
echo 1024 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/XGA/wWidth
echo 768 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/XGA/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/XGA/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/XGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/VGA
echo 7 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/VGA/wHeight
echo 333333 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/VGA/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/VGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/mjpeg/mjpeg.ss functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/uncompressed/uncompressed.ss functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/header/ss functions/uvc.usb0/streaming/class/ss

mkdir functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/mjpeg/mjpeg.hs functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/uncompressed/uncompressed.hs functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/header/hs functions/uvc.usb0/streaming/class/hs

mkdir functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/mjpeg/mjpeg.hs functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/uncompressed/uncompressed.hs functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/header/fs functions/uvc.usb0/streaming/class/fs

mkdir functions/uvc.usb0/control/header/h
ln -s functions/uvc.usb0/control/header/h functions/uvc.usb0/control/class/fs
ln -s functions/uvc.usb0/control/header/h functions/uvc.usb0/control/class/ss

cat <<EOF > functions/uvc.usb0/control/terminal/camera/default/bmControls
10
34
0
EOF

cat <<EOF > functions/uvc.usb0/control/processing/default/bmControls
91
21
EOF

mkdir configs/c.1
ln -s functions/uvc.usb0 configs/c.1
ls /sys/class/udc | grep dwc3 > UDC

sleep 1

/usr/bin/ipcuvc-ift -p ${streaming_maxpacket} -b ${streaming_maxburst} &

exit 0
