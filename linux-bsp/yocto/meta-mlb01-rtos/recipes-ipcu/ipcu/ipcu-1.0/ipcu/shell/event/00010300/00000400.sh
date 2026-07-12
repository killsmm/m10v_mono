#!/bin/sh

echo "play error" > /dev/ttyUSI0

echo $1 >> /tmp/play_error.txt
echo  >> /tmp/play_error.txt
