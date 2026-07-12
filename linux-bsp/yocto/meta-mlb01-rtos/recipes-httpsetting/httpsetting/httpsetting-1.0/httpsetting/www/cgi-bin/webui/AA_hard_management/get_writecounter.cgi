#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}


#Set variable
#echo camera_if_direct 0x81011100 0x0100 >> $LOGF
#RESULT=$(camera_if_direct 0x81011100 0x0100)
CMD="camera_if_direct 0x81011100 0x0100"
echo $CMD >> $LOGF
RESULT=$($CMD)

echo COMMAND RESPONSE:$RESULT >> $LOGF

#RESPONSE 
RES1=$(echo $RESULT | cut -c 21-30)
RES2=$(echo $RESULT | cut -c 11-20)
RES3=$(echo $RESULT | cut -c 30-40)

jo -p counter="$RES1" res2="$RES2" res3="$RES3" 
