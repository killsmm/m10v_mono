#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Date
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

#OD Function Area Conf
TMP=$(echo "$QUERY_STRING" | jq ".arg1")
FNC=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg2")
CH=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg3")
ARA=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg4")
EN=`echo $TMP | sed -e 's/\"//g'`
#OD Area Conf
TMP=$(echo "$QUERY_STRING" | jq ".arg5")
VERNUM=`echo $TMP | sed -e 's/\"//g'`
#OD Threshold Conf
TMP=$(echo "$QUERY_STRING" | jq ".arg6")
THR=`echo $TMP | sed -e 's/\"//g'`
#OD Vertex Conf
TMP=$(echo "$QUERY_STRING" | jq ".arg7")
VERNO=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg8")
X=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg9")
Y=`echo $TMP | sed -e 's/\"//g'`

#OD Function Area Conf
CMD="camera_if_direct 0x01010D00 0x00000800 $FNC $CH $ARA $EN"
echo $CMD >> $LOGF
RESULT=$($CMD)
#Response format
#Receive response. Return 1 = 0x000000XX Return 2 = 0x00000000 Return 3 = 0x00000000 Return 4 = 0x00000000 0x01010D00:0x00000800 succesfully. Camera IF command end.
#if command is (echo $RESULT | cut -c 19-39),RES1 is Return 1 = 0x000000XX
#if command is (echo $RESULT | cut -c 41-61),RES2 is Return 2 = 0x000000XX
#if command is (echo $RESULT | cut -c 63-83),RES3 is Return 3 = 0x000000XX
#if command is (echo $RESULT | cut -c 85-105),RES4 is Return 4 = 0x000000XX
#RES1=$(echo $RESULT | cut -c 30-39)
#RES2=$(echo $RESULT | cut -c 52-61)
#RES3=$(echo $RESULT | cut -c 74-83)
#RES4=$(echo $RESULT | cut -c 96-105)


#OD Area Conf
CMD="camera_if_direct 0x01010D00 0x00000A00 $VERNUM"
echo $CMD >> $LOGF
RESULT=$($CMD)
#Response format
#Receive response. Return 1 = 0x000000XX Return 2 = 0x00000000 Return 3 = 0x00000000 Return 4 = 0x00000000 0x01010D00:0x00000A00 succesfully. Camera IF command end.
#if command is (echo $RESULT | cut -c 19-39),RES1 is Return 1 = 0x000000XX
#if command is (echo $RESULT | cut -c 41-61),RES2 is Return 2 = 0x000000XX
#if command is (echo $RESULT | cut -c 63-83),RES3 is Return 3 = 0x000000XX
#if command is (echo $RESULT | cut -c 85-105),RES4 is Return 4 = 0x000000XX
#RES1=$(echo $RESULT | cut -c 30-39)
#RES2=$(echo $RESULT | cut -c 52-61)
#RES3=$(echo $RESULT | cut -c 74-83)
#RES4=$(echo $RESULT | cut -c 96-105)

#OD Threshold Conf
CMD="camera_if_direct 0x01010D00 0x00000B00 $THR"
echo $CMD >> $LOGF
RESULT=$($CMD)
#Response format
#Receive response. Return 1 = 0x000000XX Return 2 = 0x00000000 Return 3 = 0x00000000 Return 4 = 0x00000000 0x01010D00:0x00000B00 succesfully. Camera IF command end.
#if command is (echo $RESULT | cut -c 19-39),RES1 is Return 1 = 0x000000XX
#if command is (echo $RESULT | cut -c 41-61),RES2 is Return 2 = 0x000000XX
#if command is (echo $RESULT | cut -c 63-83),RES3 is Return 3 = 0x000000XX
#if command is (echo $RESULT | cut -c 85-105),RES4 is Return 4 = 0x000000XX
#RES1=$(echo $RESULT | cut -c 30-39)
#RES2=$(echo $RESULT | cut -c 52-61)
#RES3=$(echo $RESULT | cut -c 74-83)
#RES4=$(echo $RESULT | cut -c 96-105)

#OD Vertex Conf
CMD="camera_if_direct 0x01010D00 0x00000C00 $VERNO $X $Y"
echo $CMD >> $LOGF
RESULT=$($CMD)
#Response format
#Receive response. Return 1 = 0x000000XX Return 2 = 0x00000000 Return 3 = 0x00000000 Return 4 = 0x00000000 0x01010D00:0x00000C00 succesfully. Camera IF command end.
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
