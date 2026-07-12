#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# REDA JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
DR=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg2")
MIN=`echo $TMP | sed -e 's/\"//g'`

#Set variable
#QP Value
#echo camera_if_direct 0x01010400 0x0600 $DR $MIN >> $LOGF
#RESULT=$(camera_if_direct 0x01010400 0x0600 $DR $MIN)
CMD="camera_if_direct 0x01010400 0x0600 $DR $MIN"
echo $CMD >> $LOGF
RESULT=$($CMD)


jo -p result="Done."

