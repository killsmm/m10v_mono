#!/bin/sh

echo Content-type: application/json; charset=UTF-8
echo 
read QUERY_STRING

JO="jo"
JQ="jq"
CAMCOMM="camera_cmd"
TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# Read and localize from json structure ------------------------------
# Read Camera Mode
CAMERA_MODE=$(echo "$QUERY_STRING" | $JQ ".camera_mode")
ISO=$(echo "$QUERY_STRING" | $JQ ".iso")

# IPCU Command: Set Image Configuration
# Sub Commnad : ISO
echo $CAMCOMM s_i_iso $ISO >> $LOGF
RESULT=$($CAMCOMM s_i_iso $ISO)

case $CAMERA_MODE in
	"1")
        ;;
    *)
        # Update module
        # IPCU Command: Simple Tool
        # Sub Commnad : Update
        # Params      : video update
        echo $CAMCOMM simple_tool_update 0 >> $LOGF
        RESULT=$($CAMCOMM simple_tool_update 0)
        ;;
esac

$JO -p Result="Done."



