#!/bin/sh

usage="
usage:
        ./usr/bin/uac_ipcu.sh -usb3 -mic -ch0 48000
        ./usr/bin/uac_ipcu.sh -usb3 -enc -ch1 44100

parameters:
        usb3 for usb3.0
        usb2 for usb2.0
        mic for MLB01 Audio Board
        enc for Lontium Encoder Board
        ch0 for audio hw channel 0
        ch1 for audio hw channel 1

             mic     enc
smart-kit    ch0     ch0
evb          ch0     ch1

examples:
smart-kit:
sh usr/bin/uac_ipcu.sh -usb3 -enc -ch0 48000
sh usr/bin/uac_ipcu.sh -usb3 -mic -ch0 48000
evb:
sh usr/bin/uac_ipcu.sh -usb3 -enc -ch1 48000
sh usr/bin/uac_ipcu.sh -usb3 -mic -ch0 48000

For environment setting, please check with Socionext FAE.
"

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ]; then
        echo "$usage"
        exit 1
fi


usb_port=2

#audio_src will cause the cmd as 0 b b or 0 b 2
audio_src=0

#audio_ch is used for category value, not real hw ch
#if param set ch0, use will send category value as 1; and category value as 2 for ch1
#it's used for the different setting of evb and smart-kit
audio_ch=0

sampling_rate=0

#while getopts u: opt
#do
#  case $opt in
#    "u")
#         case "${OPTARG}" in
#           "sb2") usb_port=2 ;;
#           "sb3") usb_port=3 ;;
#         esac ;;
#  esac
#done

if [ $1 = -usb3 ] ; then
	usb_port=3
else
	usb_port=2
fi

if [ $2 = -mic ] ; then
	audio_src=1
else
	audio_src=2
fi

if [ $3 = -ch0 ] ; then
        audio_ch=1
else
        audio_ch=2
fi

sampling_rate=$4

if [ ${sampling_rate} = 0 ] ; then
        echo "$usage"
        exit 1
fi

if [ ${audio_src} = 0 ] ; then
        echo "$usage"
        exit 1
fi

echo "audio src: " $audio_src
echo "audio ch(for category): " $audio_ch
echo "sampling rate: " $sampling_rate

if [ ${usb_port} = 2 ] ; then
  insmod /lib/modules/$(uname -r)/kernel/drivers/usb/gadget/udc/sn_usb20_udc.ko
fi

if [ ${usb_port} = 3 ] ; then
  insmod /lib/modules/4.4.15/kernel/drivers/usb/dwc3/dwc3-sn.ko hostmode=0
fi


insmod /lib/modules/$(uname -r)/kernel/sound/soundcore.ko
insmod /lib/modules/$(uname -r)/kernel/sound/core/snd.ko
insmod /lib/modules/$(uname -r)/kernel/sound/core/snd-timer.ko
insmod /lib/modules/$(uname -r)/kernel/sound/core/snd-pcm.ko

insmod /lib/modules/$(uname -r)/kernel/fs/configfs/configfs.ko
insmod /lib/modules/$(uname -r)/kernel/drivers/usb/gadget/libcomposite.ko
insmod /lib/modules/$(uname -r)/kernel/drivers/usb/gadget/function/usb_f_uac1_mic.ko

mount -t configfs none /sys/kernel/config/
cd /sys/kernel/config/usb_gadget/
mkdir -p g1
cd g1
mkdir -p strings/0x0409

echo 0x1D6B > idVendor
echo 0x0101 > idProduct
echo 64 > bMaxPacketSize0
echo Linux Foundation > strings/0x0409/manufacturer
echo Linux USB Audio Gadget > strings/0x0409/product
echo 01234567890123 > strings/0x0409/serialnumber
echo 0xEF > bDeviceClass
echo 0x02 > bDeviceSubClass
echo 0x01 > bDeviceProtocol

mkdir functions/uac1.usb0

echo 48000,32000,24000,16000,12000,8000 > functions/uac1.usb0/p_srate
echo 256 > functions/uac1.usb0/req_number

mkdir configs/c.1
ln -s functions/uac1.usb0 configs/c.1
if [ ${usb_port} = 2 ] ; then
  udc_name="usb20dev"
else
  udc_name="dwc3"
fi
ls /sys/class/udc | grep ${udc_name} > UDC

sleep 1
echo "start to run uac-gadget"
/usr/bin/ipcuac ${audio_src} ${audio_ch} ${sampling_rate} &

exit 0

