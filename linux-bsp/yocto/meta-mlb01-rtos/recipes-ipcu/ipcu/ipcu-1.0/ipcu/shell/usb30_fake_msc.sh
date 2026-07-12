#!/bin/sh

size=50M

if [ "$1" ]; then
        echo "input size:" $1
        size=$1
fi

insmod /lib/modules/4.4.15/kernel/drivers/usb/dwc3/dwc3-sn.ko hostmode=0

### Fake block device
echo "create a fake device"
dd if=/dev/zero of=/dev/aa bs=$size count=1
busybox du -sh /dev/aa
sleep 1
losetup.util-linux -fP /dev/aa
losetup -a
sleep 1
busybox mkfs.vfat /dev/aa

### User Setting
# Device descriptor Setting
bcd_usb=0x0310
id_vendor=0x2B88
id_product=0x0004
bcd_device=0x0001
language_id=0x0409
sManufacture="Socionext Inc"
sProduct="SN-Dev"
sSerialnumber="01234567890123"

# Configuration descriptor Setting
sConfiguration="Config FSG"
bm_attributes=0xC0  # self power
max_power=250

### Start MSC Function
# create temporary script for device attach
echo "#!/bin/sh" > /tmp/insert_fsg
echo "" >> /tmp/insert_fsg
s1="echo "
s2="/dev/aa"
s3=" > /sys/kernel/config/usb_gadget/g1/functions/mass_storage.usb0/lun.0/file"
echo ${s1}${s2}${s3} >> /tmp/insert_fsg
chmod 755 /tmp/insert_fsg
mv /tmp/insert_fsg /tmp/insert_fsg.sh

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

mkdir -p functions/mass_storage.usb0
echo 1 > functions/mass_storage.usb0/stall
echo 0 > functions/mass_storage.usb0/lun.0/nofua
echo 0 > functions/mass_storage.usb0/lun.0/cdrom
echo 0 > functions/mass_storage.usb0/lun.0/ro
echo 1 > functions/mass_storage.usb0/lun.0/removable
echo "/dev/aa" > functions/mass_storage.usb0/lun.0/file

# Create Configuration Instance
mkdir -p configs/c.1/strings/${language_id}
echo ${sConfiguration} > configs/c.1/strings/${language_id}/configuration
echo ${bm_attributes} > configs/c.1/bmAttributes
echo ${max_power} > configs/c.1/MaxPower


ln -s functions/mass_storage.usb0 configs/c.1/
echo 19c00000.dwc3 > UDC
