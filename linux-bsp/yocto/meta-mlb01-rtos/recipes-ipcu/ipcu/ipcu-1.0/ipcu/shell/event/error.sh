#!/bin/sh

echo "camera_if event receive error occurred" > /dev/ttyUSI0

echo $1 >> /tmp/error.txt
echo $2 >> /tmp/error.txt
echo $3 >> /tmp/error.txt
echo $4 >> /tmp/error.txt
echo $5 >> /tmp/error.txt
echo $6 >> /tmp/error.txt
echo  >> /tmp/error.txt
