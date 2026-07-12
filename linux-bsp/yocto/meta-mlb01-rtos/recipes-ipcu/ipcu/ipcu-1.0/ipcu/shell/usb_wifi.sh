#!/bin/sh

if lsmod | grep dwc3_sn > /dev/null; then
	echo "dwc3 alread inserted, remove it"
        rmmod dwc3_sn
fi

echo "insert dwc3 as hostmode"
insmod /lib/modules/4.4.15/kernel/drivers/usb/dwc3/dwc3-sn.ko hostmode=1
sleep 3

if lsmod | grep rtl8821cu > /dev/null; then
	echo "rtl8821cu loaded!"
else
	echo "insert rtl8821cu module..."
	insmod /lib/modules/4.4.15/kernel/drivers/net/wireless/realtek/rtl8821cu/rtl8821cu.ko
	sleep 2
fi

sleep 2
if [ "$1" == "dfs" ] ; then
	echo "dfs mode"
	hostapd /etc/hostapd_wlan5g_dfs.conf &
else
	echo "normal mode"
	hostapd /etc/hostapd_wlan5g.conf &
fi

ifconfig wlan0 192.168.2.1 netmask 255.255.255.0
busybox udhcpd -f /etc/udhcpd_wlan.conf &

