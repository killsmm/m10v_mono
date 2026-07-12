#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Date
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
DFRM=`echo $TMP | sed -e 's/\"//g'`

#PD Draw Frame
#echo camera_if_direct 0x01010C00 0x200 $DFRM>> $LOGF
#RESULT=$(camera_if_direct 0x01010C00 0x200 $DFRM)
CMD="camera_if_direct 0x01010C00 0x200 $DFRM"
echo $CMD >> $LOGF
RESULT=$($CMD)
#Response format
#Receive response. Return 1 = 0x000000XX Return 2 = 0x00000000 Return 3 = 0x00000000 Return 4 = 0x00000000 0x81010C00:0x200 succesfully. Camera IF command end.
#if command is (echo $RESULT | cut -c 19-39),RES1 is Return 1 = 0x000000XX
#if command is (echo $RESULT | cut -c 41-61),RES2 is Return 2 = 0x000000XX
#if command is (echo $RESULT | cut -c 63-83),RES3 is Return 3 = 0x000000XX
#if command is (echo $RESULT | cut -c 85-105),RES4 is Return 4 = 0x000000XX
#RES1=$(echo $RESULT | cut -c 30-39)
#RES2=$(echo $RESULT | cut -c 52-61)
#RES3=$(echo $RESULT | cut -c 74-83)
#RES4=$(echo $RESULT | cut -c 96-105)

jo -p result="Done."
#jo -p result="$RES1" res2="$RES2" res3="$RES3" res4="$RES4"
