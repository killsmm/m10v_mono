#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# Data Expansion
HTMLVAL=$(echo "$QUERY_STRING" | jq ".jsval")
MENU_PARAM=`echo $HTMLVAL | sed -e 's/\"//g'`

if [ $MENU_PARAM -ne 0 ]; then
 echo camera_if_direct 0x00020100 0x100 $MENU_PARAM >> $LOGF
 RESULT=$(camera_if_direct 0x00020100 0x100 $MENU_PARAM)
else
 RESULT=0
fi

jo -p result="Done."
