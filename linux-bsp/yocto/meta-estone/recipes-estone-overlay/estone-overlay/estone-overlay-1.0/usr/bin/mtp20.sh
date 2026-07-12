#!/bin/sh

umount /media/root/test_mtp_sd/
rm -rf /media/root/test_mtp_sd
mkdir -p /media/root/fw_upgrade
mount -t ipcufs mnt /media/root/fw_upgrade -ofs=1


insmod /lib/modules/`uname -r`/kernel/drivers/usb/gadget/udc/sn_usb20_udc.ko

### User Setting
# Device descriptor Setting
bcd_usb=0x0310 
id_vendor=0x9998
id_product=0x8888
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

# Create HID Function
mkdir -p functions/hid.usb0
echo 1 > functions/hid.usb0/protocol
echo 1 > functions/hid.usb0/subclass
echo 9 > functions/hid.usb0/report_length
#echo -ne \\x05\\x01\\x09\\x06\\xa1\\x01\\x05\\x07\\x19\\xe0\\x29\\xe7\\x15\\x00\\x25\\x01\\x75\\x01\\x95\\x08\\x81\\x02\\x95\\x01\\x75\\x08\\x81\\x03\\x95\\x05\\x75\\x01\\x05\\x08\\x19\\x01\\x29\\x05\\x91\\x02\\x95\\x01\\x75\\x03\\x91\\x03\\x95\\x06\\x75\\x08\\x15\\x00\\x25\\x65\\x05\\x07\\x19\\x00\\x29\\x65\\x81\\x00\\xc0 > functions/hid.usb0/report_desc
echo -ne \\x06\\x00\\xff\\x09\\x01\\xa1\\x01\\x85\\x06\\x09\\x01\\x15\\x00\\x26\\xff\\x00\\x95\\x08\\x75\\x08\\x81\\x02\\x09\\x01\\x95\\x08\\x91\\x02\\xc0 > functions/hid.usb0/report_desc



# Create Configuration Instance
mkdir -p configs/c.1/strings/${language_id}
echo ${sConfiguration} > configs/c.1/strings/${language_id}/configuration 
echo ${bm_attributes} > configs/c.1/bmAttributes
echo ${max_power} > configs/c.1/MaxPower

# Bind Function Instance to Configuration Instance
ln -s functions/mtp.usb0 configs/c.1/
ln -s functions/hid.usb0 configs/c.1/
# Available UDC drivers
echo 1b050000.usb20dev > UDC

sleep 1
/usr/bin/m10v_hid_updater &

#for i in {1..10}
#do
#	echo "$i"
#	ps | grep /usr/bin/estone-app | grep -v "grep"
#	if [ $? -eq 0 ]; then
#		echo "start estone-app succeed"
#		exit 0
#	else
#		echo "start estone-app failed"
#		/usr/bin/estone-app &
#		sleep 1
#	fi
#done
#echo "start estone-app failed for 10 times"
#exit 1
