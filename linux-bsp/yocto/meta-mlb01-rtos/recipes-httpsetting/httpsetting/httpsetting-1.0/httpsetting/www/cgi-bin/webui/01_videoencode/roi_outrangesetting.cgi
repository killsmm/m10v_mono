#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
EN=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg2")
UP=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg3")
MAX=`echo $TMP | sed -e 's/\"//g'`

#Set variable
#OutrangeSetting
#echo camera_if_direct 0x01010400 0x0800 $EN $UP $MAX >> $LOGF
#RESULT=$(camera_if_direct 0x01010400 0x0800 $EN $UP $MAX)
CMD="camera_if_direct 0x01010400 0x0800 $EN $UP $MAX"
echo $CMD >> $LOGF
RESULT=$($CMD)


jo -p result="Done."

