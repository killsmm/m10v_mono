#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

HTMLVAL=$(echo "$QUERY_STRING" | jq ".arg1")
MENU_PARAM=`echo $HTMLVAL | sed -e 's/\"//g'`

#Set variable
#echo camera_if_direct 0x00010100 0x0700 $MENU_PARAM >> $LOGF
#RESULT=$(camera_if_direct 0x00010100 0x0700 $MENU_PARAM)
CMD="camera_if_direct 0x00010100 0x0700 $MENU_PARAM"
echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."
