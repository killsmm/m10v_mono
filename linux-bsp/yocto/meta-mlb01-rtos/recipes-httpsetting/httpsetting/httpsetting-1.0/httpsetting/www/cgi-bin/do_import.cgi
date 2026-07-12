#!/bin/sh

echo Content-type: application/json; charset=UTF-8
echo 

JO="jo"
JQ="jq"
CAMCOMM="camera_cmd"
TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# IPCU Command: Simple Tool
# Sub Commnad : Read SD
echo $CAMCOMM simple_tool_read_sd >> $LOGF
RESULT=$($CAMCOMM simple_tool_read_sd)

# IPCU Command: Simple Tool
# Sub Commnad : Update
# Params      : Camera_Mode設定と対応する値
echo $CAMCOMM simple_tool_update 0 >> $LOGF
RESULT=$($CAMCOMM simple_tool_update 0)

$JO -p Result="Done."
