#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
ROT=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg2")
MR=`echo $TMP | sed -e 's/\"//g'`


#Enabled
#echo camera_if_direct 0x01010700 0x100 $ROT $MR >> $LOGF
#RESULT=$(camera_if_direct 0x01010700 0x100 $ROT $MR)
CMD="camera_if_direct 0x01010700 0x0100 $ROT $MR"
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

