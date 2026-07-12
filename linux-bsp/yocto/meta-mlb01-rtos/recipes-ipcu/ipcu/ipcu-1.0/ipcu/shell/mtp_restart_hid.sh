#!/bin/sh

busybox killall sn-hid
echo '' > /sys/kernel/config/usb_gadget/g1/UDC

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

# Create Gadget Device Instance
echo ${bcd_usb} > bcdUSB
echo ${id_vendor} > idVendor
echo ${id_product} > idProduct
echo ${bcd_device} > bcdDevice
mkdir -p strings/${language_id}
echo ${sManufacture} > strings/${language_id}/manufacturer
echo ${sProduct} > strings/${language_id}/product
echo ${sSerialnumber} > strings/${language_id}/serialnumber

# Create Configuration Instance
mkdir -p configs/c.1/strings/${language_id}
echo ${sConfiguration} > configs/c.1/strings/${language_id}/configuration
echo ${max_power} > configs/c.1/MaxPower
echo ${bm_attributes} > configs/c.1/bmAttributes

## mtp
echo "mount partition..."
rm -rf /media/root/test_mtp_sd/
mkdir -p /media/root/mtp
mount -t ipcufs mnt /media/root/mtp -ofs=1
cd /sys/kernel/config/usb_gadget/g1

# Create Function Instance
mkdir -p functions/mtp.usb0


# Bind Function Instance to Configuration Instance
ln -s functions/mtp.usb0 configs/c.1/

echo 19c00000.dwc3 > UDC

echo "running sn-hid"
sleep 3
sn-hid &