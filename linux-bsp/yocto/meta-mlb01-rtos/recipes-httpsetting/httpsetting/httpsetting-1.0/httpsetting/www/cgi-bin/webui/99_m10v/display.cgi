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
 0 ) HDMI=0 LCD=0 CAMERA=1;;
 1 ) HDMI=1 LCD=0 CAMERA=1;;
 2 ) HDMI=1 LCD=0 CAMERA=2;;
 3 ) HDMI=1 LCD=0 CAMERA=3;;
 4 ) HDMI=1 LCD=0 CAMERA=4;;
 5 ) HDMI=0 LCD=1 CAMERA=1;;
 6 ) HDMI=0 LCD=1 CAMERA=2;;
 7 ) HDMI=0 LCD=1 CAMERA=3;;
 8 ) HDMI=0 LCD=1 CAMERA=4;;
esac

#Set variable
#echo camera_if_direct 0x01010100 0x100 $HDMI $LCD $CAMERA >> $LOGF
#RESULT=$(camera_if_direct 0x01010100 0x100 $HDMI $LCD $CAMERA)
CMD="camera_if_direct 0x01010100 0x100 $HDMI $LCD $CAMERA"
echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."
