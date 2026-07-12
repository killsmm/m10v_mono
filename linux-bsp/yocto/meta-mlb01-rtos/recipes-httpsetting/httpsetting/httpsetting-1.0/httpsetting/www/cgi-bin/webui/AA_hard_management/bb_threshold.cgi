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
BB_TH=`echo $TMP | sed -e 's/\"//g'`

#Set variable
#echo camera_if_direct 0x00040500 0x100 $BB_TH >> $LOGF
#RESULT=$(camera_if_direct 0x00040500 0x100 $BB_TH)
CMD="camera_if_direct 0x00040500 0x100 $BB_TH"
echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."
