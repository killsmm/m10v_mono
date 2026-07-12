#!/bin/sh

echo "Panorama updated : Progress event (/tmp/panorama_progress)" > /dev/ttyUSI0

echo $1 >> /tmp/panorama_progress.txt
echo  >> /tmp/panorama_progress.txt
