#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Date
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
GCD=`echo $TMP | sed -e 's/\"//g'`

# GCD Threshold
CMD="camera_if_direct 0x01010E00 0x00000300 $GCD"
echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."
