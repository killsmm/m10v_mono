#!/bin/sh

streaming_maxpacket=1024
streaming_maxburst=3
usb_port=2
surv=0
stream_id=0
roi=0
bulk=0
fps_debug_en=0
#debug_gnso2
max_payload_size=49152
max_payload_size_host=57344
with_header=1
header_size=16

while getopts u:s:p:b:i:r:x:d:l:m:w:h: opt
do
  case $opt in
    "u")
         case "${OPTARG}" in
           "sb2") usb_port=2 ;;
           "sb3") usb_port=3 ;;
         esac ;;
    "s")
         case "${OPTARG}" in
           "urv") surv=1 ;;
         esac ;;
    "p") streaming_maxpacket="$OPTARG" ;;
    "b") streaming_maxburst="$OPTARG" ;;
    "i") stream_id="$OPTARG" ;;
    "r") roi="$OPTARG" ;;
    "x") bulk="$OPTARG" ;;
    "d") fps_debug_en="$OPTARG" ;;
    #debug_gnso2
    "l") max_payload_size="$OPTARG" ;;
    "m") max_payload_size_pc="$OPTARG" ;;
    "w") with_header="$OPTARG" ;;
    "h") header_size="$OPTARG" ;;
  esac
done

if [ ${bulk} = 1 ] ; then
       echo "bulk mode"
       echo 1 > /tmp/bulk.conf
       streaming_maxburst=15
else
        echo "iso mode"
fi

if [ ${fps_debug_en} = 1 ] ; then
	echo "fps debug enable"
	echo 1 > /tmp/uvc_fps.conf
fi

if [ ${roi} = 1 ] ; then
	echo "roi mode"
	echo 1 > /tmp/roi.conf
else
	echo "non-roi mode"
fi

#debug_gnso
echo ${max_payload_size} >> /tmp/max_payload_size.conf
echo ${max_payload_size_host} >> /tmp/max_payload_size_host.conf
echo ${with_header} >> /tmp/with_header.conf
echo ${header_size} >> /tmp/header_size.conf

[ ${streaming_maxpacket} -gt 3072 ] && exit 1
[ ${streaming_maxpacket} -lt 1 ] && exit 1
[ ${streaming_maxburst} -gt 15 ] && exit 1
[ ${streaming_maxburst} -lt 0 ] && exit 1

if [ ${usb_port} = 2 ] ; then
  product="SN-UVC2"
  insmod /lib/modules/`uname -r`/kernel/drivers/usb/gadget/udc/sn_usb20_udc.ko
else
  product="SN-UVC3"
  insmod /lib/modules/4.4.15/kernel/drivers/usb/dwc3/dwc3-sn.ko hostmode=0
fi
echo "1"
modprobe libcomposite
mount -t configfs none /sys/kernel/config/
cd /sys/kernel/config/usb_gadget/
mkdir g1
cd g1
mkdir strings/0x0409

echo 0x1D6B > idVendor
echo 0x0104 > idProduct
echo 1024 > bMaxPacketSize0
echo 239 > bDeviceClass
echo 2 > bDeviceSubClass
echo 1 > bDeviceProtocol
echo Socionext Inc > strings/0x0409/manufacturer
echo ${product} > strings/0x0409/product
echo 01234567890123 > strings/0x0409/serialnumber
echo "22"
mkdir functions/uvc.usb0

echo 1 > functions/uvc.usb0/streaming_interval
echo ${streaming_maxpacket} > functions/uvc.usb0/streaming_maxpacket
echo ${streaming_maxburst} > functions/uvc.usb0/streaming_maxburst

#default mode (non roi)
if [ ${roi} = 0 ] ; then
echo "3"
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

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD
echo 1 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD
echo 2 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/wHeight
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD
echo 3 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/wHeight
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/dwDefaultFrameInterval


mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss

if [ ${bulk} = 1 ] ; then
mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/wHeight
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD
echo 2 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wHeight
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwDefaultFrameInterval

else

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwDefaultFrameInterval

fi

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.ss

mkdir functions/uvc.usb0/streaming/h264/h264.ss/UHD
echo 1 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.ss/FHD
echo 2 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/wHeight
echo 166666 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.ss/HD
echo 3 > functions/uvc.usb0/streaming/h264/h264.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/h264/h264.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/h264/h264.ss/HD/wHeight
echo 166666 > functions/uvc.usb0/streaming/h264/h264.ss/HD/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/h264/h264.ss/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.ss/VGA
echo 4 > functions/uvc.usb0/streaming/h264/h264.ss/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/h264/h264.ss/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/h264/h264.ss/VGA/wHeight
echo 166666 > functions/uvc.usb0/streaming/h264/h264.ss/VGA/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/h264/h264.ss/VGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs

mkdir functions/uvc.usb0/streaming/h264/h264.hs/UHD
echo 1 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs/FHD
echo 2 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/wHeight
echo 166666 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs/HD
echo 3 > functions/uvc.usb0/streaming/h264/h264.hs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/h264/h264.hs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/h264/h264.hs/HD/wHeight
echo 166666 > functions/uvc.usb0/streaming/h264/h264.hs/HD/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/h264/h264.hs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs/VGA
echo 4 > functions/uvc.usb0/streaming/h264/h264.hs/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/h264/h264.hs/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/h264/h264.hs/VGA/wHeight
echo 166666 > functions/uvc.usb0/streaming/h264/h264.hs/VGA/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/h264/h264.hs/VGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/mjpeg/mjpeg.ss functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/uncompressed/uncompressed.ss functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/h264/h264.ss functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/header/ss functions/uvc.usb0/streaming/class/ss

mkdir functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/mjpeg/mjpeg.hs functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/uncompressed/uncompressed.hs functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/h264/h264.hs functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/header/hs functions/uvc.usb0/streaming/class/hs

mkdir functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/mjpeg/mjpeg.hs functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/uncompressed/uncompressed.hs functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/h264/h264.hs functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/header/fs functions/uvc.usb0/streaming/class/fs

mkdir functions/uvc.usb0/control/header/h
ln -s functions/uvc.usb0/control/header/h functions/uvc.usb0/control/class/fs
ln -s functions/uvc.usb0/control/header/h functions/uvc.usb0/control/class/ss

cat <<EOF > functions/uvc.usb0/control/terminal/camera/default/bmControls
10
138
0
EOF

cat <<EOF > functions/uvc.usb0/control/processing/default/bmControls
91
20
EOF

else

# roi mode
mkdir functions/uvc.usb0/streaming/h264/h264.ss

mkdir functions/uvc.usb0/streaming/h264/h264.ss/UHD
echo 1 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.ss/FHD
echo 2 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.ss/HD
echo 3 > functions/uvc.usb0/streaming/h264/h264.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/h264/h264.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/h264/h264.ss/HD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/HD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/HD/dwDefaultFrameInterval



mkdir functions/uvc.usb0/streaming/h264/h264.hs

mkdir functions/uvc.usb0/streaming/h264/h264.hs/UHD
echo 1 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs/FHD
echo 2 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs/HD
echo 3 > functions/uvc.usb0/streaming/h264/h264.hs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/h264/h264.hs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/h264/h264.hs/HD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/HD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/h264/h264.ss functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/header/ss functions/uvc.usb0/streaming/class/ss

mkdir functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/h264/h264.hs functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/header/hs functions/uvc.usb0/streaming/class/hs

mkdir functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/h264/h264.hs functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/header/fs functions/uvc.usb0/streaming/class/fs

mkdir functions/uvc.usb0/control/header/h
ln -s functions/uvc.usb0/control/header/h functions/uvc.usb0/control/class/fs
ln -s functions/uvc.usb0/control/header/h functions/uvc.usb0/control/class/ss

cat <<EOF > functions/uvc.usb0/control/terminal/camera/default/bmControls
10
138
0
EOF

cat <<EOF > functions/uvc.usb0/control/processing/default/bmControls
91
20
EOF

fi

mkdir configs/c.1
ln -s functions/uvc.usb0 configs/c.1
if [ ${usb_port} = 2 ] ; then
  udc_name="usb20dev"
else
  udc_name="dwc3"
fi
ls /sys/class/udc | grep ${udc_name} > UDC

sleep 1
echo "run..."
/usr/bin/ipcuvc -p ${streaming_maxpacket} -b ${streaming_maxburst} -i ${stream_id} &

exit 0
