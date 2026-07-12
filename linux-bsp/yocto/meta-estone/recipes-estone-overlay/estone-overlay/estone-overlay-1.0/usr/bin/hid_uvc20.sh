#!/bin/sh
#mount -t ipcufs mnt /media/root/test_mtp_sd -ofs=8

streaming_maxpacket=1024
streaming_maxburst=0
usb_port=2
surv=0
stream_id=0
id_vendor=0x9998
id_product=0x8888

[ ${streaming_maxpacket} -gt 3072 ] && exit 1
[ ${streaming_maxpacket} -lt 1 ] && exit 1
[ ${streaming_maxburst} -gt 15 ] && exit 1
[ ${streaming_maxburst} -lt 0 ] && exit 1


product="SN-UVC2"
insmod /lib/modules/`uname -r`/kernel/drivers/usb/gadget/udc/sn_usb20_udc.ko


modprobe libcomposite
mount -t configfs none /sys/kernel/config/
cd /sys/kernel/config/usb_gadget/
mkdir g1
cd g1
mkdir strings/0x0409

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


# SUPER SPEED MJPEG
mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss

#mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD
#echo 1 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/bFrameIndex
#echo 3840 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/wWidth
#echo 2160 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/wHeight
#echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/dwFrameInterval
#echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD
echo 2 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD
echo 3 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA
echo 4 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/CIF
echo 5 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/CIF/bFrameIndex
echo 352 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/CIF/wWidth
echo 288 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/CIF/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/CIF/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/CIF/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QVGA
echo 6 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QVGA/bFrameIndex
echo 320 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QVGA/wWidth
echo 240 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QVGA/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QVGA/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QVGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QCIF
echo 7 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QCIF/bFrameIndex
echo 176 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QCIF/wWidth
echo 144 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QCIF/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QCIF/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/QCIF/dwDefaultFrameInterval

# HIGH SPEED MJPEG 
mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs

#mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD
#echo 1 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/bFrameIndex
#echo 3840 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/wWidth
#echo 2160 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/wHeight
#echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/dwFrameInterval
#echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD
echo 2 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD
echo 3 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA
echo 4 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/CIF
echo 5 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/CIF/bFrameIndex
echo 352 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/CIF/wWidth
echo 288 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/CIF/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/CIF/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/CIF/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QVGA
echo 6 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QVGA/bFrameIndex
echo 320 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QVGA/wWidth
echo 240 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QVGA/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QVGA/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QVGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QCIF
echo 7 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QCIF/bFrameIndex
echo 176 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QCIF/wWidth
echo 144 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QCIF/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QCIF/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/QCIF/dwDefaultFrameInterval

# SUPER SPEED YUYV
mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD
echo 2 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA
echo 3 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/dwDefaultFrameInterval


# HIGH SPEED YUYV
mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/FHD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/FHD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/FHD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/VGA
echo 2 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/VGA/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/VGA/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/VGA/dwDefaultFrameInterval

# SUPER SPEED H.264
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

mkdir functions/uvc.usb0/streaming/h264/h264.ss/CIF
echo 5 > functions/uvc.usb0/streaming/h264/h264.ss/CIF/bFrameIndex
echo 352 > functions/uvc.usb0/streaming/h264/h264.ss/CIF/wWidth
echo 288 > functions/uvc.usb0/streaming/h264/h264.ss/CIF/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/CIF/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/CIF/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.ss/QVGA
echo 6 > functions/uvc.usb0/streaming/h264/h264.ss/QVGA/bFrameIndex
echo 320 > functions/uvc.usb0/streaming/h264/h264.ss/QVGA/wWidth
echo 240 > functions/uvc.usb0/streaming/h264/h264.ss/QVGA/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/QVGA/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/QVGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.ss/QCIF
echo 7 > functions/uvc.usb0/streaming/h264/h264.ss/QCIF/bFrameIndex
echo 176 > functions/uvc.usb0/streaming/h264/h264.ss/QCIF/wWidth
echo 144 > functions/uvc.usb0/streaming/h264/h264.ss/QCIF/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/QCIF/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/QCIF/dwDefaultFrameInterval

# HIGH SPEED H.264
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

mkdir functions/uvc.usb0/streaming/h264/h264.hs/CIF
echo 5 > functions/uvc.usb0/streaming/h264/h264.hs/CIF/bFrameIndex
echo 352 > functions/uvc.usb0/streaming/h264/h264.hs/CIF/wWidth
echo 288 > functions/uvc.usb0/streaming/h264/h264.hs/CIF/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/CIF/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/CIF/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs/QVGA
echo 6 > functions/uvc.usb0/streaming/h264/h264.hs/QVGA/bFrameIndex
echo 320 > functions/uvc.usb0/streaming/h264/h264.hs/QVGA/wWidth
echo 240 > functions/uvc.usb0/streaming/h264/h264.hs/QVGA/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/QVGA/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/QVGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs/QCIF
echo 7 > functions/uvc.usb0/streaming/h264/h264.hs/QCIF/bFrameIndex
echo 176 > functions/uvc.usb0/streaming/h264/h264.hs/QCIF/wWidth
echo 144 > functions/uvc.usb0/streaming/h264/h264.hs/QCIF/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/QCIF/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/QCIF/dwDefaultFrameInterval

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



mkdir configs/c.1

udc_name="usb20dev"

#ls /sys/class/udc | grep ${udc_name} > UDC



### User Setting
# Device descriptor Setting
bcd_usb=0x0200
bcd_device=0x0001
language_id=0x0409
sManufacture="Socionext Inc"
sProduct="SN-MTP"
sSerialnumber="01234567890122"

# Configuration descriptor Setting
sConfiguration="Config uvc"
bm_attributes=0xC0  # self power
max_power=120

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
mkdir -p functions/hid.usb0
echo 1 > functions/hid.usb0/protocol
echo 1 > functions/hid.usb0/subclass
echo 9 > functions/hid.usb0/report_length
#echo -ne \\x05\\x01\\x09\\x06\\xa1\\x01\\x05\\x07\\x19\\xe0\\x29\\xe7\\x15\\x00\\x25\\x01\\x75\\x01\\x95\\x08\\x81\\x02\\x95\\x01\\x75\\x08\\x81\\x03\\x95\\x05\\x75\\x01\\x05\\x08\\x19\\x01\\x29\\x05\\x91\\x02\\x95\\x01\\x75\\x03\\x91\\x03\\x95\\x06\\x75\\x08\\x15\\x00\\x25\\x65\\x05\\x07\\x19\\x00\\x29\\x65\\x81\\x00\\xc0 > functions/hid.usb0/report_desc
echo -ne \\x06\\x00\\xff\\x09\\x01\\xa1\\x01\\x85\\x06\\x09\\x01\\x15\\x00\\x26\\xff\\x00\\x95\\x08\\x75\\x08\\x81\\x02\\x09\\x01\\x95\\x08\\x91\\x02\\xc0 > functions/hid.usb0/report_desc

# mtp function

#umount /media/root/test_mtp_sd/
#rm -rf /media/root/test_mtp_sd
#mkdir -p /media/root/fw_upgrade
#mount -t ipcufs mnt /media/root/fw_upgrade -ofs=1
#mkdir -p functions/mtp.usb0



# Create Configuration Instance
mkdir -p configs/c.1/strings/${language_id}
echo ${sConfiguration} > configs/c.1/strings/${language_id}/configuration 
echo ${bm_attributes} > configs/c.1/bmAttributes
echo ${max_power} > configs/c.1/MaxPower

# Bind Function Instance to Configuration Instance


#ln -s functions/mtp.usb0 configs/c.1/
ln -s functions/uvc.usb0 configs/c.1
ln -s functions/hid.usb0 configs/c.1/

# Available UDC drivers
echo 1b050000.usb20dev > UDC



#/usr/bin/estone-app &

#sleep 1

/usr/bin/ipcuvc -s -p ${streaming_maxpacket} -b ${streaming_maxburst} -i ${stream_id} &
/usr/bin/hid_controller /dev/hidg0 &

exit 0

