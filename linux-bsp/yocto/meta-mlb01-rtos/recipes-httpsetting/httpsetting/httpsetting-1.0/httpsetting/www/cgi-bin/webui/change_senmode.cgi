#!/bin/sh

echo Content-type: application/json; charset=UTF-8
echo 
read QUERY_STRING

CAMIF="camera_if"
TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}
# Data Expansion
CAMERA_MODE=$(echo "$QUERY_STRING" | jq ".camera_mode")

echo $CAMIF chg_camera_mode $CAMERA_MODE >> $LOGF
RESULT=$($CAMIF chg_camera_mode $CAMERA_MODE)

# 20170629 Sugaya $JO -> jo
jo -p result="Done."

