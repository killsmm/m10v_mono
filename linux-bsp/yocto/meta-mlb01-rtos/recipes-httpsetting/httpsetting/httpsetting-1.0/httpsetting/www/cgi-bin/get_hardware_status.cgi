#!/bin/sh

echo Content-type: application/json; charset=UTF-8
echo 
read QUERY_STRING

JO="jo"
JQ="jq"
CAMCOMM="camera_cmd"
TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# IPCU Command: Get Hardware status
# Sub Commnad : SD Card
# Return      : Connected(1) or Not connected(0)
echo $CAMCOMM -r 1 g_sd_card >> $LOGF
HW_STATUS=$($CAMCOMM -r 1 g_sd_card)

# Regist ack returns in json structure
JO_HARDWARE=$($JO hw_status=$HW_STATUS)
$JO -p Status=$JO_HARDWARE
