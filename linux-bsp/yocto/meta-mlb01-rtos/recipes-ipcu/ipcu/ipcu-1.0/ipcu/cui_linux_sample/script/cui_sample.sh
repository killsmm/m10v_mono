#! /bin/sh

kill `ps | grep camera_Ev_rec | awk '{print $1}'`

if [ $1 = "preview" ]; then
	$1
elif [ $1 = "single_capture" ]; then
	$1 $2 $3 $4 $5
elif [ $1 = "burst_capture" ]; then
	$1 $2 $3 $4 $5 $6 $7
elif [ $1 = "video_capture" ]; then
	$1 $2 $3 $4
elif [ $1 = "media_format" ]; then
	$1
fi
