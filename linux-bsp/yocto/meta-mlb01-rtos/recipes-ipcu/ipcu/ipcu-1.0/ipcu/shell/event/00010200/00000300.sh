#!/bin/sh

echo "Playback Finished" > /dev/ttyUSI0

echo $1 >> /tmp/playback_finished.txt
echo $2 >> /tmp/playback_finished.txt
echo  >> /tmp/playback_finished.txt
