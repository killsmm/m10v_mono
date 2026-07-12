#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# Data  Expansion
HTMLVAL=$(echo "$QUERY_STRING" | jq ".jsval")
MENU_PARAM=`echo $HTMLVAL | sed -e 's/\"//g'`

echo camera_if_direct 0x00010800 0x100 $MENU_PARAM >> $LOGF
RESULT=$(camera_if_direct 0x00010800 0x100 $MENU_PARAM)

jo -p result="Done."
