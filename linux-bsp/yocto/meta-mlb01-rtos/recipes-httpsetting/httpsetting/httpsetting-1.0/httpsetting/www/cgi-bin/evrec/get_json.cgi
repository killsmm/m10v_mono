#!/bin/sh

echo Content-type: text/html; charset=UTF-8
echo
read evtype

if [ $evtype = "NBB" ]; then
	if [ -f /tmp/NBB.json ]; then
		num_nbb=$(cat /tmp/NBB.json | jq ".num_bad_block")
		echo $num_nbb | sed -e 's/"//g'
	else
		echo "Event not happened."
	fi

elif [ $evtype = "SSC" ]; then
	if [ -f /tmp/SSC.json ]; then
		sen_stat=$(cat /tmp/SSC.json | jq ".sensor_status")
		echo $sen_stat | sed -e 's/"//g'
	else
		echo "Event not happened."
	fi

else
	echo "Error"
fi
