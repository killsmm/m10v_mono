#!/bin/sh

echo "video record error" > /dev/ttyUSI0

echo $1 >> /tmp/video_rec_error.txt
echo  >> /tmp/video_rec_error.txt
