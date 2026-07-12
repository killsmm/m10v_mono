#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

HTMLVAL1=$(echo "$QUERY_STRING" | jq ".arg1")
HTMLVAL2=$(echo "$QUERY_STRING" | jq ".arg2")
HTMLVAL3=$(echo "$QUERY_STRING" | jq ".arg3")
HTMLVAL4=$(echo "$QUERY_STRING" | jq ".arg4")
MENU_PARAM1=`echo $HTMLVAL1 | sed -e 's/\"//g'`
MENU_PARAM2=`echo $HTMLVAL2 | sed -e 's/\"//g'`
MENU_PARAM3=`echo $HTMLVAL3 | sed -e 's/\"//g'`
MENU_PARAM4=`echo $HTMLVAL4 | sed -e 's/\"//g'`

#Set variable
#echo camera_if_direct 0x00010100 0x0400 $MENU_PARAM1 $MENU_PARAM2 $MENU_PARAM3 $MENU_PARAM4 >> $LOGF
#RESULT=$(camera_if_direct 0x00010100 0x0400 $MENU_PARAM1 $MENU_PARAM2 $MENU_PARAM3 $MENU_PARAM4)
CMD="camera_if_direct 0x00010100 0x0400 $MENU_PARAM1 $MENU_PARAM2 $MENU_PARAM3 $MENU_PARAM4"
echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."
