#!/bin/sh

echo Content-type: application/json; charset=UTF-8
echo 

JO="jo"
JQ="jq"
CAMCOMM="camera_cmd"
TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# IPCU Command: Simple Tool
# Sub Commnad : Save SD
echo $CAMCOMM simple_tool_save_sd >> $LOGF
RESULT=$($CAMCOMM simple_tool_save_sd)

$JO -p Result="Done."