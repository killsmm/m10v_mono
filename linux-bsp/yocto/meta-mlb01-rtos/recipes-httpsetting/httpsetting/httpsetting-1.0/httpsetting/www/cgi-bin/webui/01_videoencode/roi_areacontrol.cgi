#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
S_NO=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg2")
A_NO=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg3")
EN=`echo $TMP | sed -e 's/\"//g'`

#Set variable
#AreaControl
#echo camera_if_direct 0x01010400 0x0300 $S_NO $A_NO $EN >> $LOGF
#RESULT=$(camera_if_direct 0x01010400 0x0300 $S_NO $A_NO $EN)
CMD="camera_if_direct 0x01010400 0x0300 $S_NO $A_NO $EN"
echo $CMD >> $LOGF
RESULT=$($CMD)


jo -p result="Done."

