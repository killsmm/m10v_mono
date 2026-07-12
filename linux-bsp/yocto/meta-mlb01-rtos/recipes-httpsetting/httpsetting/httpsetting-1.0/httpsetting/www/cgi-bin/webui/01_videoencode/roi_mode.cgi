#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
MD=`echo $TMP | sed -e 's/\"//g'`

#Set variable
#MODE
#echo camera_if_direct 0x01010400 0x0100 $MD >> $LOGF
#RESULT=$(camera_if_direct 0x01010400 0x0100 $MD)
CMD="camera_if_direct 0x01010400 0x0100 $MD"
echo $CMD >> $LOGF
RESULT=$($CMD)


jo -p result="Done."

