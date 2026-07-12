#!/bin/sh
insmod /lib/modules/4.4.15/kernel/drivers/usb/dwc3/dwc3-sn.ko hostmode=0

### User Setting
# Device descriptor Setting
bcd_usb=0x0310 
id_vendor=0x2B88
id_product=0x0001
bcd_device=0x0001
language_id=0x0409
sManufacture="Socionext Inc"
sProduct="SN-MTP"
sSerialnumber="01234567890123"

# Configuration descriptor Setting
sConfiguration="Config MTP"
bm_attributes=0xC0  # self power
max_power=250

### Start MTP Function
# Mounting USB Gadget ConfigFS
modprobe libcomposite
if [ ! -d /sys/kernel/config/usb_gadget ] ; then
mount -t configfs none /sys/kernel/config/
fi
cd /sys/kernel/config/usb_gadget/
mkdir mtp
cd mtp

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
mkdir -p functions/mtp.usb0

# Create Configuration Instance
mkdir -p configs/c.1/strings/${language_id}
echo ${sConfiguration} > configs/c.1/strings/${language_id}/configuration 
echo ${bm_attributes} > configs/c.1/bmAttributes
echo ${max_power} > configs/c.1/MaxPower

# Bind Function Instance to Configuration Instance
ln -s functions/mtp.usb0 configs/c.1/

# Available UDC drivers
echo 19c00000.dwc3 > UDC


