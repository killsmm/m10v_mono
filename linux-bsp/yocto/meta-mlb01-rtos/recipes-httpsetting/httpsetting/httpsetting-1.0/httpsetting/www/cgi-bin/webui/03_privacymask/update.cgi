#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
XCOOR=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg2")
YCOOR=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg3")
WIDTH=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg4")
HEIGHT=`echo $TMP | sed -e 's/\"//g'`

#Set variable
#echo camera_if_direct 0x01010500 0x300 $XCOOR $YCOOR $WIDTH $HEIGHT >> $LOGF
#RESULT=$(camera_if_direct 0x01010500 0x300 $XCOOR $YCOOR $WIDTH $HEIGHT)
CMD="camera_if_direct 0x01010500 0x300 $XCOOR $YCOOR $WIDTH $HEIGHT"
echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."
