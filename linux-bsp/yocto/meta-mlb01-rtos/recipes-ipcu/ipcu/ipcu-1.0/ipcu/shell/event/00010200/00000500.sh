#!/bin/sh

echo "movie restore finish" > /dev/ttyUSI0

echo $1 >> /tmp/movie_restore_finish.txt
echo  >> /tmp/movie_restore_finish.txt
