#!/bin/sh

echo Content-type: application/json; charset=UTF-8
echo 
read QUERY_STRING

JO="jo"
JQ="jq"
CAMCOMM="camera_cmd"
TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

WB_MODE=$(echo "$QUERY_STRING" | $JQ ".wb_mode")

# IPCU Command: Set Image Configuration
# Sub Commnad : White Balance (Mode)
# TODO        : ManualのParam1,2は？
echo $CAMCOMM s_i_white_balance $WB_MODE 256 256 >> $LOGF
RESULT=$($CAMCOMM s_i_white_balance $WB_MODE 256 256)

$JO -p Result="Done."


