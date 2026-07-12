#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

#Param is none

#Software Reset
#echo camera_if_direct 0x00000200 0x100 >> $LOGF
#RESULT=$(camera_if_direct 0x00000200 0x100)
CMD="camera_if_direct 0x00000200 0x100"
echo $CMD >> $LOGF
RESULT=$($CMD)


jo -p result="Done."

