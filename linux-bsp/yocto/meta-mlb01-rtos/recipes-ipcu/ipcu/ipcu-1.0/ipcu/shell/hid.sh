#!/bin/sh

usb_port=$1
desc_sel=0

usage="
usage:
        sh /usr/bin/hid.sh -usb3

parameters:
	-usb3	: 	for usb3.0 port
	-usb2	:	for usb2.0 port
	-s	:	0 for default descriptor (default value)
			1 for hp descriptor"

if [ -z "$1" ]; then
        echo "$usage"
        exit 1
fi


while getopts u:s: opt
do
  case $opt in
    "u")
         case "${OPTARG}" in
           "sb2") usb_port=2 ;;
           "sb3") usb_port=3 ;;
         esac ;;
    "s") desc_sel="$OPTARG" ;;
  esac
done

echo "usb_port:" ${usb_port}
echo "desc_sel:" ${desc_sel}

if [ ${usb_port} = 2 ] ; then
  echo "USB2"
  insmod /lib/modules/4.4.15/kernel/drivers/usb/gadget/udc/sn_usb20_udc.ko
  bcd_usb=0x0200
else
  echo "USB3"
  bcd_usb=0x0310
  insmod /lib/modules/4.4.15/kernel/drivers/usb/dwc3/dwc3-sn.ko hostmode=0
fi

### User Setting
# Device descriptor Setting
bcd_usb=0x0310
id_vendor=0x1D6B
id_product=0x0104 #0xa4ac
bcd_device=0x0100
language_id=0x0409
sManufacture="Socionext Inc"
sProduct="SN-HID"
sSerialnumber="01234567890123"

# Configuration descriptor Setting
sConfiguration="Config HID"
bm_attributes=0xC0  # self power
max_power=250


# Mounting USB Gadget ConfigFS
modprobe libcomposite
if [ ! -d /sys/kernel/config/usb_gadget ] ; then
mount -t configfs none /sys/kernel/config/
fi
cd /sys/kernel/config/usb_gadget/
mkdir g1
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
mkdir -p functions/hid.usb0
echo 1 > functions/hid.usb0/protocol
echo 0 > functions/hid.usb0/subclass

case "$desc_sel" in
0)
echo 63 > functions/hid.usb0/report_length
echo -ne \\x05\\x01\\x09\\x06\\xa1\\x01\\x05\\x07\\x19\\xe0\\x29\\xe7\\x15\\x00\\x25\\x01\\x75\\x01\\x95\\x08\\x81\\x02\\x95\\x01\\x75\\x08\\x81\\x03\\x95\\x05\\x75\\x01\\x05\\x08\\x19\\x01\\x29\\x05\\x91\\x02\\x95\\x01\\x75\\x03\\x91\\x03\\x95\\x06\\x75\\x08\\x15\\x00\\x25\\x65\\x05\\x07\\x19\\x00\\x29\\x65\\x81\\x00\\xc0 > functions/hid.usb0/report_desc
echo "common desc"
;;
1)
echo "hp desc"
echo 64 > functions/hid.usb0/report_length
echo -ne \\x06\\x00\\xff\\x09\\x01\\xa1\\x01\\x85\\x01\\x09\\x01\\x15\\x00\\x26\\xff\\x00\\x95\\x3f\\x75\\x08\\x81\\x02\\x85\\x01\\x09\\x01\\x91\\x02\\xc0 > functions/hid.usb0/report_desc
;;
*) echo "selection $desc_sel : unknow." ;;
esac


# Create Configuration Instance
mkdir -p configs/c.1/strings/${language_id}
echo ${sConfiguration} > configs/c.1/strings/${language_id}/configuration
echo ${max_power} > configs/c.1/MaxPower
echo ${bm_attributes} > configs/c.1/bmAttributes

# Bind Function Instance to Configuration Instance
ln -s functions/hid.usb0 configs/c.1/

# Available UDC drivers
if [ ${usb_port} = 2 ] ; then
  echo 1b050000.usb20dev > UDC
else
  echo 19c00000.dwc3 > UDC
fi

echo "running sn-hid"
sleep 3
sn-hid &

