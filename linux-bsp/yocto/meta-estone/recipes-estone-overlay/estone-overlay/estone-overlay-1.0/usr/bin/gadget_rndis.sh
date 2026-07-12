#!/bin/sh
DEVICE_TYPE=rndis
if [ -n "$1" ]; then
	DEVICE_TYPE=$1
fi
echo "${DEVICE_TYPE}"

mount -t configfs none /sys/kernel/config/
cd /sys/kernel/config/usb_gadget/
mkdir g1
cd g1

mkdir strings/0x0409

echo 0x1D6B >idVendor
echo 0x0104 >idProduct
echo 128 >bMaxPacketSize0
echo 239 >bDeviceClass
echo 2 >bDeviceSubClass
echo 1 >bDeviceProtocol
echo estone >strings/0x0409/manufacturer
echo camera >strings/0x0409/product
echo 01234567890123 >strings/0x0409/serialnumber
mkdir functions/"${DEVICE_TYPE}".usb0
#echo 2 > functions/${DEVICE_TYPE}.usb0/qmult

mkdir -p configs/c.1/strings/0x0409
#echo ${sConfiguration} > configs/c.1/strings/${language_id}/configuration
#echo ${max_power} > configs/c.1/MaxPower
#echo ${bm_attributes} > configs/c.1/bmAttributes

ln -s functions/"${DEVICE_TYPE}".usb0 configs/c.1/

ls /sys/class/udc > UDC
