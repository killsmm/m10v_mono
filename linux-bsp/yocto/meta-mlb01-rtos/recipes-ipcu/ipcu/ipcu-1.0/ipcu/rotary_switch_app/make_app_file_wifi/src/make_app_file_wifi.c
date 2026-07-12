/******************************************************************************/
/*
/*  Copyright 2018 Socionext Inc.
/*
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "make_app_file_wifi.h"


int main(int argc, char *argv[])
{
	FILE *file;
	file = fopen("/tmp/app_file_wifi.sh","w");
	fprintf(file,"#!/bin/sh\n");
	fprintf(file,"\n");
	fprintf(file,"# kill connman\n");
	fprintf(file,"echo \"kill connman\" > /dev/ttyUSI0\n");
	fprintf(file,"kill -9 `ps | grep [c]onnmand | awk '{print $1}'`\n");
	fprintf(file,"sleep 1s\n");
	fprintf(file,"\n");
	fprintf(file,"# release soft block\n");
	fprintf(file,"echo \"release soft block\" > /dev/ttyUSI0\n");
	fprintf(file,"busybox rfkill unblock 0\n");
	fprintf(file,"busybox rfkill unblock 0\n");
	fprintf(file,"sleep 1s\n");
	fprintf(file,"busybox rfkill list all\n");
	fprintf(file,"sleep 1s\n");
	fprintf(file,"\n");
	fprintf(file,"# attach WiFi driver\n");
	fprintf(file,"echo \"attach WiFi driver\" > /dev/ttyUSI0\n");
	fprintf(file,"modprobe mwifiex_sdio\n");
	fprintf(file,"sleep 3s\n");
	fprintf(file,"\n");
	fprintf(file,"# start AP mode\n");
	fprintf(file,"echo \"start AP mode\" > /dev/ttyUSI0\n");
	fprintf(file,"iw dev mlan0 interface add uap0 type __ap\n");
	fprintf(file,"sleep 3s\n");
	fprintf(file,"cp /run/SD/mmcblk0p1/hostapd.conf /etc/\n");
	fprintf(file,"hostapd /etc/hostapd.conf &\n");
	fprintf(file,"sleep 3s\n");
	fprintf(file,"ifconfig uap0 192.168.2.1 netmask 255.255.255.0\n");
	fprintf(file,"sleep 3s\n");
	fprintf(file,"echo \"start DHCP\" > /dev/ttyUSI0\n");
	fprintf(file,"busybox udhcpd -f /etc/udhcpd.conf &\n");
	fprintf(file,"sleep 1s\n");
	fprintf(file,"\n");
	fprintf(file,"# start httpd\n");
	fprintf(file,"echo \"start httpd\" > /dev/ttyUSI0\n");
	fprintf(file,"cp -r /run/SD/mmcblk0p1/sdk/usr /\n");
	fprintf(file,"cp -r /run/SD/mmcblk0p1/sdk/www /var/\n");
	fprintf(file,"busybox httpd -h /var/www/ -c /etc/httpd2.conf\n");
	fprintf(file,"sleep 1s\n");
	fprintf(file,"\n");
	fprintf(file,"# tool initialize\n");
	fprintf(file,"echo \"tool initialize\" > /dev/ttyUSI0\n");
	fprintf(file,"sh /var/www/tool_init.sh\n");
	fclose(file);


	return 0;
}
