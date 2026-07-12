#!/bin/sh

echo "sd status changed" > /dev/ttyUSI0

echo $1 >> /tmp/sd_status.txt
echo  >> /tmp/sd_status.txt
