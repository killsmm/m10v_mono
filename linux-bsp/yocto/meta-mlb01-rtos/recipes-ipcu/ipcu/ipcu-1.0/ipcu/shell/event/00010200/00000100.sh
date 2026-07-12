#!/bin/sh

echo "image capture finish" > /dev/ttyUSI0

echo $1 >> /tmp/image_cap_finish.txt
echo  >> /tmp/image_cap_finish.txt
