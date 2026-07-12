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
TMP=$(echo "$QUERY_STRING" | jq ".arg5")
ARG5=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg6")
ARG6=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg7")
ARG7=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg8")
ARG8=`echo $TMP | sed -e 's/\"//g'`

CMD="osdcmd $ARG1,$ARG2,$ARG3,$ARG4,$ARG5,$ARG8,$ARG6,$ARG7"
echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."

