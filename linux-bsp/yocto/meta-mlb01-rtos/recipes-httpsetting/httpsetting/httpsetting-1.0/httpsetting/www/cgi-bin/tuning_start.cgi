#!/bin/sh

echo Content-type: application/json; charset=UTF-8
echo 

JO="jo"
JQ="jq"
CAMCOMM="camera_cmd"
TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# IPCU Command: Change Camera Mode
# Sub Commnad : Change Mode
# Params      : IDLE Mode
echo $CAMCOMM chg_camera_mode 0x1 >> $LOGF
RESULT=$($CAMCOMM chg_camera_mode 0x1)

# IPCU Command: Simple Tool
# Sub Commnad : Start
echo $CAMCOMM simple_tool_start >> $LOGF
RESULT=$($CAMCOMM simple_tool_start)

$JO -p Result="Done."
