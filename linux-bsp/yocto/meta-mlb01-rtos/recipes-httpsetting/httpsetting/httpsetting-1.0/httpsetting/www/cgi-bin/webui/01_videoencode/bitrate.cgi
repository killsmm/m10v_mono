#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Date
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
CH=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg2")
AVE=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg3")
MAX=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg4")
ALG=`echo $TMP | sed -e 's/\"//g'`

#Set variable
#Bitrate
#echo camera_if_direct 0x00010300 0x200 $CH $AVE $MAX $ALG >> $LOGF
#RESULT=$(camera_if_direct 0x00010300 0x200 $CH $AVE $MAX $ALG)
CMD="camera_if_direct 0x00010300 0x0200 $CH $AVE $MAX $ALG"
echo $CMD >> $LOGF
RESULT=$($CMD)


jo -p result="Done."

#READVAL=$(echo `camera_if_direct 0x80010300 0x200` | cut -c19-)
#P1=$(echo $READVAL | cut -c 12-21)
#P2=$(echo $READVAL | cut -c 34-43)
#P3=$(echo $READVAL | cut -c 56-65)
#P4=$(echo $READVAL | cut -c 78-87)
#
#echo camera_if_direct 0x10300 0x200 0x00 $AVE $VAL $P4 >> $LOGF
#RESULT=$(camera_if_direct 0x10300 0x200 0x00 $AVE $VAL $P4)

