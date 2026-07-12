#!/bin/sh

streaming_maxpacket=1024
streaming_maxburst=0
usb_port=2
surv=0
stream_id=0

while getopts u:s:p:b:i: opt
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
  esac
done

[ ${streaming_maxpacket} -gt 3072 ] && exit 1
[ ${streaming_maxpacket} -lt 1 ] && exit 1
[ ${streaming_maxburst} -gt 15 ] && exit 1
[ ${streaming_maxburst} -lt 0 ] && exit 1

if [ ${usb_port} = 2 ] ; then
  product="SN-UVC2"
  insmod /lib/modules/`uname -r`/kernel/drivers/usb/gadget/udc/sn_usb20_udc.ko
else
  product="SN-UVC3"
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

mkdir functions/uvc.usb0

echo 1 > functions/uvc.usb0/streaming_interval
echo ${streaming_maxpacket} > functions/uvc.usb0/streaming_maxpacket
echo ${streaming_maxburst} > functions/uvc.usb0/streaming_maxburst

if [ ${surv} = 0 ] ; then

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/uncompressed/uncompressed.ss functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/header/ss functions/uvc.usb0/streaming/class/ss

mkdir functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/uncompressed/uncompressed.hs functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/header/hs functions/uvc.usb0/streaming/class/hs

mkdir functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/uncompressed/uncompressed.hs functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/header/fs functions/uvc.usb0/streaming/class/fs

mkdir functions/uvc.usb0/control/header/h
ln -s functions/uvc.usb0/control/header/h functions/uvc.usb0/control/class/fs
ln -s functions/uvc.usb0/control/header/h functions/uvc.usb0/control/class/ss

cat <<EOF > functions/uvc.usb0/control/terminal/camera/default/bmControls
10
0
0
EOF

cat <<EOF > functions/uvc.usb0/control/processing/default/bmControls
91
20
EOF

else

echo "Sorry...YUV is unsupported format."
exit 1

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

/usr/bin/ipcuvc -f 0 -p ${streaming_maxpacket} -b ${streaming_maxburst} -i ${stream_id} &

exit 0
