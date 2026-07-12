#!/bin/sh

device=/dev/sdb
MP=/mnt

# you can override these by env vars

if [ -z "$CROSS_COMPILE" ] ; then
	CROSS_COMPILE=arm-linux-gnueabihf- 
fi
if [ -z "$KO_DIR" ] ; then
	KO_DIR=../../panda
fi

# won't be used if don't exist

LMP_REFLASH=usb-Linaro_Ltd_LavaLMP_LL04phone-pwr-01-if00
LMP_SDMUX=usb-Linaro_Ltd_LavaLMP_LL0aabcdef000201-if00 

# end of user-serviceable parts

LMP_MODE=
if [ -e /dev/serial/by-id/$LMP_REFLASH ] ; then
	LMP_MODE=1
	echo "LMP mode"
fi
DIR=mb86s27


if [ ! -z "$LMP_MODE" ] ; then
	sudo echo -n -e "\x02{\"schema\":\"org.linaro.lmp.usb\",\"modes\":[{\"name\":\"usb\",\"option\":\"host\"}]}\x04" > /dev/serial/by-id/$LMP_REFLASH
	sudo echo -n -e "\x02{\"schema\":\"org.linaro.lmp.sdmux\",\"modes\":[{\"name\":\"dut\",\"option\":\"disconnect\"},{\"name\":\"host\",\"option\":\"uSDA\"}]}" > /dev/serial/by-id/$LMP_SDMUX

	echo "(1/3) Mounting"
	sleep 5s
fi

sudo umount $device?
sudo mount $device""1 $MP
if [ $? -ne 0 ] ; then
	exit 1
fi

sudo cp -f $DIR/stubby-mb86s27.bin-* $MP/DATA.bin && \
sudo cp -f $DIR/nand.par $DIR/SDRAM.PAR $MP && \
sudo umount $MP
sudo umount $device? 2>/dev/null

if [ ! -z "$LMP_MODE" ] ; then

	sudo echo -n -e "\x02{\"schema\":\"org.linaro.lmp.sdmux\",\"modes\":[{\"name\":\"dut\",\"option\":\"uSDA\"},{\"name\":\"host\",\"option\":\"disconnect\"}]}" > /dev/serial/by-id/$LMP_SDMUX
 
	# he's programming
	echo "(2/3) Programming to NAND"
	sleep 3s

	sudo echo -n -e "\x02{\"schema\":\"org.linaro.lmp.usb\",\"modes\":[{\"name\":\"usb\",\"option\":\"disconnect\"}]}\x04" > /dev/serial/by-id/$LMP_REFLASH
sleep 0.5s
	sudo echo -n -e "\x02{\"schema\":\"org.linaro.lmp.sdmux\",\"modes\":[{\"name\":\"dut\",\"option\":\"disconnect\"},{\"name\":\"host\",\"option\":\"uSDA\"}]}" > /dev/serial/by-id/$LMP_SDMUX
	sudo echo -n -e "\x02{\"schema\":\"org.linaro.lmp.sdmux\",\"modes\":[{\"name\":\"dut\",\"option\":\"uSDA\"},{\"name\":\"host\",\"option\":\"disconnect\"}]}" > /dev/serial/by-id/$LMP_SDMUX

	echo "(3/3) Booting"
fi

