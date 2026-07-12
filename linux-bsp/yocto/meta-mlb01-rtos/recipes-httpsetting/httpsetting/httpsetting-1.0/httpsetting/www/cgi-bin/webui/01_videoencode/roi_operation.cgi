#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
OP=`echo $TMP | sed -e 's/\"//g'`

#Set variable
#OPERATION
#echo camera_if_direct 0x01010400 0x0200 $OP >> $LOGF
#RESULT=$(camera_if_direct 0x01010400 0x0200 $OP)
CMD="camera_if_direct 0x01010400 0x0200 $OP"
echo $CMD >> $LOGF
RESULT=$($CMD)


jo -p result="Done."

