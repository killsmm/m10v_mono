#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# Data Expansion
TMP=$(echo "$QUERY_STRING" | jq ".arg1")
ARG1=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg2")
ARG2=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg3")
ARG3=`echo $TMP | sed -e 's/\"//g'`

#Set variavle
CMD="camera_if_direct 0x01010500 0x200 $ARG1 $ARG2 $ARG3"
echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."
