#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
ARG1=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg2")
ARG2=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg3")
ARG3=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg4")
ARG4=`echo $TMP | sed -e 's/\"//g'`

#Set variavle
#echo camera_if_direct 0x00010300 0x100 $ARG1 $ARG2 $ARG3 $ARG4>> $LOGF
#RESULT=$(camera_if_direct 0x00010300 0x100 $ARG1 $ARG2 $ARG3 $ARG4)
CMD="camera_if_direct 0x00010300 0x0100 $ARG1 $ARG2 $ARG3 $ARG4"
echo $CMD >> $LOGF
RESULT=$($CMD)


jo -p result="Done."
