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
MENU_PARAM=`echo $HTMLVAL | sed -e 's/\"//g'`

case $MENU_PARAM in
 0 ) AVE=32000 ;;
 1 ) AVE=40000 ;;
 2 ) AVE=48000 ;;
 3 ) AVE=64000 ;;
 4 ) AVE=96000 ;;
 5 ) AVE=112000;;
 6 ) AVE=160000;;
 7 ) AVE=192000;;
esac

#Set variable
#echo camera_if_direct 0x10200 0x300 $AVE >> $LOGF
#RESULT=$(camera_if_direct 0x10200 0x300 $AVE)
CMD="camera_if_direct 0x10200 0x300 $AVE"
echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."
