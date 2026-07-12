#!/bin/sh

echo "video capture finish" > /dev/ttyUSI0

echo $1 >> /tmp/video_cap_finish.txt
echo $2 >> /tmp/video_cap_finish.txt
echo  >> /tmp/video_cap_finish.txt
