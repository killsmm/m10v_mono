#!/bin/sh
if [ ! $1 ]
then
echo "USAGE: ./start_sshd.sh [192.168.1.223]"
exit 1
fi

echo "start to config network"
net_init
ifconfig eth0 $1
ssh-keygen -A
mkdir -p /var/run/sshd
/usr/sbin/sshd


