#!/bin/sh
echo 1 > /tmp/1011.conf
#chmod 0777 /usr/bin/ipcuvc
streaming_maxpacket=1024
streaming_maxburst=3
usb_port=3
surv=0
stream_id=0
roi=0
bulk=0
fps_debug_en=0
rtospl=0
max_payload_size=49152
max_payload_size_host=57344
with_header=1
header_size=16
in_dev=2
rtospl_iso=0

while getopts u:s:p:b:i:r:x:d:l:m:w:h:o:e:c: opt
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
    "l") max_payload_size="$OPTARG" ;;
    "m") max_payload_size_pc="$OPTARG" ;;
    "w") with_header="$OPTARG" ;;
    "h") header_size="$OPTARG" ;;
    "e") in_dev="$OPTARG" ;;
    "c") rtospl_iso="$OPTARG" ;;
  esac
done




if [ ${bulk} = 1 ] ; then
       echo "bulk mode"
       echo 1 > /tmp/bulk.conf
       streaming_maxburst=15
       rtospl=1
else
        echo "iso mode"
fi

if [ ${rtospl_iso} = 1 ] ; then
	streaming_maxburst=15
	rtospl=1
fi

if [ ${rtospl} = 1 ] ; then
       echo "using rtos pl"
       echo 1 > /tmp/rtospl.conf

else
        echo "not using rtol pl"
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

#default mode (non roi)
if [ ${roi} = 0 ] ; then
echo "3"

##################################################
# MJPEG                                          #
# 1. UHD  (3840x2160) p30                        #
# 2. WQHD (2560x1440) p60/p30                    #
# 3. FHD  (1920x1080) p60/p59.94/p29.97/p23.98   #
# 4. HD    (1280x720) p60/p59.94                 #
# 5. NTSC   (720x480) p60/p59.94                 #
# 6. VGA    (640x480) p60/p59.94                 #
##################################################

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss

# MJPEG UHD (3840x2160) p30
mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD
echo 1 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/UHD/dwDefaultFrameInterval

# MJPEG WQHD (2560x1440) p60/p30
mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/WQHD
echo 2 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/WQHD/bFrameIndex
echo 2560 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/WQHD/wWidth
echo 1440 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/WQHD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/WQHD/dwFrameInterval
166666
333333
EOF
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/WQHD/dwDefaultFrameInterval

# MJPEG FHD (1920x1080) p60/p59.94/p29.97/p23.98
mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD
echo 3 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/dwFrameInterval
166666
166833
333667
417014
EOF
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/FHD/dwDefaultFrameInterval

# MJPEG HD (1280x720) p60/p59.94
mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD
echo 4 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/HD/dwDefaultFrameInterval

# MJPEG NTSC (720x480) p60/p59.94
mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/NTSC
echo 5 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/NTSC/bFrameIndex
echo 720 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/NTSC/wWidth
echo 480 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/NTSC/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/NTSC/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/NTSC/dwDefaultFrameInterval

# MJPEG VGA (640x480) p60/p59.94
mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA
echo 6 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.ss/VGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD
echo 1 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/WQHD
echo 2 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/WQHD/bFrameIndex
echo 2560 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/WQHD/wWidth
echo 1440 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/WQHD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/WQHD/dwFrameInterval
166666
333333
EOF
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/WQHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD
echo 3 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/dwFrameInterval
166666
166833
333667
417014
EOF
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD
echo 4 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/NTSC
echo 5 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/NTSC/bFrameIndex
echo 720 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/NTSC/wWidth
echo 480 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/NTSC/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/NTSC/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/NTSC/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA
echo 6 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.hs/VGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.fs

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/UHD
echo 1 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/UHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/UHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/WQHD
echo 2 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/WQHD/bFrameIndex
echo 2560 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/WQHD/wWidth
echo 1440 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/WQHD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/WQHD/dwFrameInterval
166666
333333
EOF
echo 333333 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/WQHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/FHD
echo 3 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/FHD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/FHD/dwFrameInterval
166666
166833
333667
417014
EOF
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/HD
echo 4 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/HD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/HD/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/NTSC
echo 5 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/NTSC/bFrameIndex
echo 720 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/NTSC/wWidth
echo 480 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/NTSC/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/NTSC/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/NTSC/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/VGA
echo 6 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/VGA/wHeight
cat <<EOF > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/VGA/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/mjpeg/mjpeg.fs/VGA/dwDefaultFrameInterval

#######################################################
# YUY2                                                #
# bulk mode                                           #
# 1. FHD  (1920x1080) p60/p59.94/p30/p29.97/p23.98    #
# 2. HD    (1280x720) p60/p59.94/p30                  #
# 3. NTSC   (720x480) p60/p59.94/p30                  #
# 4. VGA    (640x480) p60/p59.94                      #
#######################################################

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss

if [ ${bulk} = 1 ] ; then
# YUY2 FHD (1920x1080) p60/p59.94/p30
mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/dwFrameInterval
166666
166833
333333
333667
417014
EOF
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/dwDefaultFrameInterval

# YUY2 HD (1280x720) p60/p59.94/p30
mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD
echo 2 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwFrameInterval
166666
166833
333333
EOF
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwDefaultFrameInterval

# YUY2 NTSC (720x480) p60/p59.94/p30
mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/NTSC
echo 3 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/NTSC/bFrameIndex
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/NTSC/wWidth
echo 480 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/NTSC/wHeight
cat <<EOF > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/NTSC/dwFrameInterval
166666
166833
333333
EOF
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/NTSC/dwDefaultFrameInterval

# YUY2 VGA (640x480) p60/p59.94
mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA
echo 4 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/wHeight
cat <<EOF > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/VGA/dwDefaultFrameInterval

else

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD
echo 2 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/wHeight
echo 333333 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/uncompressed/uncompressed.ss/HD/dwDefaultFrameInterval

fi

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.hs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.fs

mkdir functions/uvc.usb0/streaming/uncompressed/uncompressed.fs/HD
echo 1 > functions/uvc.usb0/streaming/uncompressed/uncompressed.fs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed/uncompressed.fs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed/uncompressed.fs/HD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.fs/HD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed/uncompressed.fs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss

if [ ${bulk} = 1 ] ; then
mkdir functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/FHD
echo 1 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/FHD/wHeight
echo 166666 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/FHD/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/HD
echo 2 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/HD/wHeight
echo 166666 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/HD/dwFrameInterval
echo 166666 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/HD/dwDefaultFrameInterval

else

mkdir functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/FHD
echo 1 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/FHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/FHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/HD
echo 2 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/HD/wHeight
echo 333333 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/HD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss/HD/dwDefaultFrameInterval

fi

mkdir functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.hs

mkdir functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.hs/HD
echo 1 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.hs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.hs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.hs/HD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.hs/HD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.hs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.fs

mkdir functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.fs/HD
echo 1 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.fs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.fs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.fs/HD/wHeight
echo 666666 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.fs/HD/dwFrameInterval
echo 666666 > functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.fs/HD/dwDefaultFrameInterval

##################################################
# h264                                           #
# 1. UHD  (3840x2160) p30                        #
# 2. WQHD (2560x1440) p30                        #
# 3. FHD  (1920x1080) p60/p59.94/p29.97/p23.98   #
# 4. HD    (1280x720) p60/p59.94                 #
# 5. NTSC   (720x480) p60/p59.94                 #
# 6. VGA    (640x480) p60/p59.94                 #
##################################################

mkdir functions/uvc.usb0/streaming/h264/h264.ss

# h264 UHD (3840x2160) p30
mkdir functions/uvc.usb0/streaming/h264/h264.ss/UHD
echo 1 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/UHD/dwDefaultFrameInterval

# h264 WQHD (2560x1440) p30
mkdir functions/uvc.usb0/streaming/h264/h264.ss/WQHD
echo 2 > functions/uvc.usb0/streaming/h264/h264.ss/WQHD/bFrameIndex
echo 2560 > functions/uvc.usb0/streaming/h264/h264.ss/WQHD/wWidth
echo 1440 > functions/uvc.usb0/streaming/h264/h264.ss/WQHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.ss/WQHD/dwFrameInterval

# h264 FHD (1920x1080) p60/p59.94/p29.97/p23.98
mkdir functions/uvc.usb0/streaming/h264/h264.ss/FHD
echo 3 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/h264/h264.ss/FHD/dwFrameInterval
166666
166833
333667
417014
EOF
echo 166666 > functions/uvc.usb0/streaming/h264/h264.ss/FHD/dwDefaultFrameInterval

# h264 HD (1280x720) p60/p59.94
mkdir functions/uvc.usb0/streaming/h264/h264.ss/HD
echo 4 > functions/uvc.usb0/streaming/h264/h264.ss/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/h264/h264.ss/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/h264/h264.ss/HD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/h264/h264.ss/HD/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/h264/h264.ss/HD/dwDefaultFrameInterval

# h264 NTSC (720x480) p60/p59.94
mkdir functions/uvc.usb0/streaming/h264/h264.ss/NTSC
echo 5 > functions/uvc.usb0/streaming/h264/h264.ss/NTSC/bFrameIndex
echo 720 > functions/uvc.usb0/streaming/h264/h264.ss/NTSC/wWidth
echo 480 > functions/uvc.usb0/streaming/h264/h264.ss/NTSC/wHeight
cat <<EOF > functions/uvc.usb0/streaming/h264/h264.ss/NTSC/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/h264/h264.ss/NTSC/dwDefaultFrameInterval

# h264 VGA (640x480) p60/p59.94
mkdir functions/uvc.usb0/streaming/h264/h264.ss/VGA
echo 6 > functions/uvc.usb0/streaming/h264/h264.ss/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/h264/h264.ss/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/h264/h264.ss/VGA/wHeight
cat <<EOF > functions/uvc.usb0/streaming/h264/h264.ss/VGA/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/h264/h264.ss/VGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs

mkdir functions/uvc.usb0/streaming/h264/h264.hs/UHD
echo 1 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs/WQHD
echo 2 > functions/uvc.usb0/streaming/h264/h264.hs/WQHD/bFrameIndex
echo 2560 > functions/uvc.usb0/streaming/h264/h264.hs/WQHD/wWidth
echo 1440 > functions/uvc.usb0/streaming/h264/h264.hs/WQHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.hs/WQHD/dwFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs/FHD
echo 3 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/h264/h264.hs/FHD/dwFrameInterval
166666
166833
333667
417014
EOF
echo 166666 > functions/uvc.usb0/streaming/h264/h264.hs/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs/HD
echo 4 > functions/uvc.usb0/streaming/h264/h264.hs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/h264/h264.hs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/h264/h264.hs/HD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/h264/h264.hs/HD/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/h264/h264.hs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs/NTSC
echo 5 > functions/uvc.usb0/streaming/h264/h264.hs/NTSC/bFrameIndex
echo 720 > functions/uvc.usb0/streaming/h264/h264.hs/NTSC/wWidth
echo 480 > functions/uvc.usb0/streaming/h264/h264.hs/NTSC/wHeight
cat <<EOF > functions/uvc.usb0/streaming/h264/h264.hs/NTSC/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/h264/h264.hs/NTSC/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.hs/VGA
echo 6 > functions/uvc.usb0/streaming/h264/h264.hs/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/h264/h264.hs/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/h264/h264.hs/VGA/wHeight
cat <<EOF > functions/uvc.usb0/streaming/h264/h264.hs/VGA/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/h264/h264.hs/VGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.fs

mkdir functions/uvc.usb0/streaming/h264/h264.fs/UHD
echo 1 > functions/uvc.usb0/streaming/h264/h264.fs/UHD/bFrameIndex
echo 3840 > functions/uvc.usb0/streaming/h264/h264.fs/UHD/wWidth
echo 2160 > functions/uvc.usb0/streaming/h264/h264.fs/UHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.fs/UHD/dwFrameInterval
echo 333333 > functions/uvc.usb0/streaming/h264/h264.fs/UHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.fs/WQHD
echo 2 > functions/uvc.usb0/streaming/h264/h264.fs/WQHD/bFrameIndex
echo 2560 > functions/uvc.usb0/streaming/h264/h264.fs/WQHD/wWidth
echo 1440 > functions/uvc.usb0/streaming/h264/h264.fs/WQHD/wHeight
echo 333333 > functions/uvc.usb0/streaming/h264/h264.fs/WQHD/dwFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.fs/FHD
echo 3 > functions/uvc.usb0/streaming/h264/h264.fs/FHD/bFrameIndex
echo 1920 > functions/uvc.usb0/streaming/h264/h264.fs/FHD/wWidth
echo 1080 > functions/uvc.usb0/streaming/h264/h264.fs/FHD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/h264/h264.fs/FHD/dwFrameInterval
166666
166833
333667
417014
EOF
echo 166666 > functions/uvc.usb0/streaming/h264/h264.fs/FHD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.fs/HD
echo 4 > functions/uvc.usb0/streaming/h264/h264.fs/HD/bFrameIndex
echo 1280 > functions/uvc.usb0/streaming/h264/h264.fs/HD/wWidth
echo 720 > functions/uvc.usb0/streaming/h264/h264.fs/HD/wHeight
cat <<EOF > functions/uvc.usb0/streaming/h264/h264.fs/HD/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/h264/h264.fs/HD/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.fs/NTSC
echo 5 > functions/uvc.usb0/streaming/h264/h264.fs/NTSC/bFrameIndex
echo 720 > functions/uvc.usb0/streaming/h264/h264.fs/NTSC/wWidth
echo 480 > functions/uvc.usb0/streaming/h264/h264.fs/NTSC/wHeight
cat <<EOF > functions/uvc.usb0/streaming/h264/h264.fs/NTSC/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/h264/h264.fs/NTSC/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/h264/h264.fs/VGA
echo 6 > functions/uvc.usb0/streaming/h264/h264.fs/VGA/bFrameIndex
echo 640 > functions/uvc.usb0/streaming/h264/h264.fs/VGA/wWidth
echo 480 > functions/uvc.usb0/streaming/h264/h264.fs/VGA/wHeight
cat <<EOF > functions/uvc.usb0/streaming/h264/h264.fs/VGA/dwFrameInterval
166666
166833
EOF
echo 166666 > functions/uvc.usb0/streaming/h264/h264.fs/VGA/dwDefaultFrameInterval

mkdir functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/mjpeg/mjpeg.ss functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/uncompressed/uncompressed.ss functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.ss functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/h264/h264.ss functions/uvc.usb0/streaming/header/ss
ln -s functions/uvc.usb0/streaming/header/ss functions/uvc.usb0/streaming/class/ss

mkdir functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/mjpeg/mjpeg.hs functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/uncompressed/uncompressed.hs functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.hs functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/h264/h264.hs functions/uvc.usb0/streaming/header/hs
ln -s functions/uvc.usb0/streaming/header/hs functions/uvc.usb0/streaming/class/hs

mkdir functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/mjpeg/mjpeg.fs functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/uncompressed/uncompressed.fs functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/uncompressed_nv12/uncompressed_nv12.fs functions/uvc.usb0/streaming/header/fs
ln -s functions/uvc.usb0/streaming/h264/h264.fs functions/uvc.usb0/streaming/header/fs
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
95
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
ln -s functions/uvc.usb0/streaming/h264/h264.fs functions/uvc.usb0/streaming/header/fs
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
95
20
EOF

fi

mkdir configs/c.1

##uac
audio_src=1
audio_ch=1

sampling_rate=48000


insmod /lib/modules/$(uname -r)/kernel/sound/soundcore.ko
insmod /lib/modules/$(uname -r)/kernel/sound/core/snd.ko
insmod /lib/modules/$(uname -r)/kernel/sound/core/snd-timer.ko
insmod /lib/modules/$(uname -r)/kernel/sound/core/snd-pcm.ko

insmod /lib/modules/$(uname -r)/kernel/drivers/usb/gadget/function/usb_f_uac1_mic.ko


cd /sys/kernel/config/usb_gadget/g1
mkdir functions/uac1.usb0

echo 48000,32000,24000,16000,12000,8000 > functions/uac1.usb0/p_srate
echo 256 > functions/uac1.usb0/req_number

ln -s functions/uvc.usb0 configs/c.1
ln -s functions/uac1.usb0 configs/c.1

sleep 2

ls /sys/class/udc | grep "dwc3" > UDC

sleep 1
echo "running uvc"
/usr/bin/ipcuvc -p ${streaming_maxpacket} -b ${streaming_maxburst} -i ${stream_id} -d ${in_dev} &
sleep 3
echo "running uac"
/usr/bin/ipcuac ${audio_src} ${audio_ch} ${sampling_rate} &


exit 0

