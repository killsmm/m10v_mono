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
HTMLVAL=$(echo "$QUERY_STRING" | jq ".arg4")
MENU_PARAM4=`echo $HTMLVAL | sed -e 's/\"//g'`
HTMLVAL=$(echo "$QUERY_STRING" | jq ".arg5")
MENU_PARAM5=`echo $HTMLVAL | sed -e 's/\"//g'`

#Set variable
#echo camera_if_direct 0x00040100 0x0200 $MENU_PARAM1 $MENU_PARAM2 $MENU_PARAM3 $MENU_PARAM4 $MENU_PARAM5 >> $LOGF
#RESULT=$(camera_if_direct 0x00040100 0x0200 $MENU_PARAM1 $MENU_PARAM2 $MENU_PARAM3 $MENU_PARAM4 $MENU_PARAM5)
CMD="camera_if_direct 0x00040100 0x0200 $MENU_PARAM1 $MENU_PARAM2 $MENU_PARAM3 $MENU_PARAM4 $MENU_PARAM5"
echo $CMD>> $LOGF
RESULT=$($CMD)

jo -p result="Done."
