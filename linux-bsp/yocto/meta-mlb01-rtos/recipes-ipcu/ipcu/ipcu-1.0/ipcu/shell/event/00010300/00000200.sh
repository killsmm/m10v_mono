#!/bin/sh

echo "image capture error" > /dev/ttyUSI0

echo $1 >> /tmp/image_cap_error.txt
echo  >> /tmp/image_cap_error.txt

