#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
SX=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg2")
SY=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg3")
EX=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg4")
EY=`echo $TMP | sed -e 's/\"//g'`

#Set variable
#Coordinate
#echo camera_if_direct 0x01010400 0x0400 $SX $SY $EX $EY >> $LOGF
#RESULT=$(camera_if_direct 0x01010400 0x0400 $SX $SY $EX $EY)
CMD="camera_if_direct 0x01010400 0x0400 $SX $SY $EX $EY"
echo $CMD >> $LOGF
RESULT=$($CMD)


jo -p result="Done."

