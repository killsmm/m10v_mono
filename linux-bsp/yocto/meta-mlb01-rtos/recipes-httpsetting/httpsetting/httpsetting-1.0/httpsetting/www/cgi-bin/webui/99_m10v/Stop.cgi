#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

#Set variable
#echo camera_if_direct 0x00040200 0x0700 >> $LOGF
#RESULT=$(camera_if_direct 0x00040200 0x0700)
CMD="camera_if_direct 0x00040200 0x0700"
echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."
