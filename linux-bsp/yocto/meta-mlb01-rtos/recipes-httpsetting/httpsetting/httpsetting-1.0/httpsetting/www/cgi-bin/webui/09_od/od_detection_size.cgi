#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Date
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
CH=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg2")
MPX=`echo $TMP | sed -e 's/\"//g'`

#OD Unmoving Conf
#echo camera_if_direct 0x01010D00 0x00000D00 $CH $MPX>> $LOGF
#RESULT=$(camera_if_direct 0x01010D00 0x00000D00 $CH $MPX)
CMD="camera_if_direct 0x01010D00 0x00000D00 $CH $MPX"
echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."
#jo -p result="$RES1" res2="$RES2" res3="$RES3" res4="$RES4"
