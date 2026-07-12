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

case $MENU_PARAM in
 0 ) SENTYPE=0x100 P1=0;;
 1 ) SENTYPE=0x100 P1=1;;
 2 ) SENTYPE=0x100 P1=2;;
 3 ) SENTYPE=0x100 P1=3;;
 4 ) SENTYPE=0x200 P1=0;;
 5 ) SENTYPE=0x200 P1=1;;
# MENU_PARAM=6(DualCamera Stitch PtnC(Fish Eye) is not used)
# 6 ) SENTYPE=0x200 P1=2;;
 7 ) SENTYPE=0x200 P1=3;;

esac

echo camera_if_direct 0x01030100 $SENTYPE $P1 >> $LOGF
RESULT=$(camera_if_direct 0x01030100 $SENTYPE $P1)

jo -p result="Done."
