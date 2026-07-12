#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# Data Expansion
HTMLVAL=$(echo "$QUERY_STRING" | jq ".arg1")
OUT_CTL=`echo $HTMLVAL | sed -e 's/\"//g'`

#Set variable
#echo camera_if_direct 0x00010200 0x900 $OUT_CTL >> $LOGF
#RESULT=$(camera_if_direct 0x00010200 0x900 $OUT_CTL)
CMD="camera_if_direct 0x00010200 0x900 $OUT_CTL"
echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."
