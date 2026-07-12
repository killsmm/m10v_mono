#!/bin/sh

echo "Time Lapse Count event (/tmp/timelapse_count)" > /dev/ttyUSI0

echo $1 >> /tmp/timelapse_count.txt
echo  >> /tmp/timelapse_count.txt
