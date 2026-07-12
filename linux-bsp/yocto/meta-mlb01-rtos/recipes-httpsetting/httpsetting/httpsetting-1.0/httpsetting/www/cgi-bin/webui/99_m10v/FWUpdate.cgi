#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

HTMLVAL=$(echo "$QUERY_STRING" | jq ".arg1")
MENU_PARAM1=`echo $HTMLVAL | sed -e 's/\"//g'`
HTMLVAL=$(echo "$QUERY_STRING" | jq ".arg2")
MENU_PARAM2=`echo $HTMLVAL | sed -e 's/\"//g'`
HTMLVAL=$(echo "$QUERY_STRING" | jq ".arg3")
MENU_PARAM3=`echo $HTMLVAL | sed -e 's/\"//g'`

#Set variable
CMD="camera_if_direct 0x00030400 0x0100 $MENU_PARAM1 $MENU_PARAM2 $MENU_PARAM3"

echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."
