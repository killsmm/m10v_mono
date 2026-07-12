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
WB_MODE=$(echo "$QUERY_STRING" | $JQ ".wb_mode")

# Change Camera Mode -------------------------------------------------
# IPCU Command: Change Camera Mode
# Sub Commnad : Change Mode
# Params      : Idle
echo $CAMCOMM chg_camera_mode 1 >> $LOGF
RESULT=$($CAMCOMM chg_camera_mode 1)

# IPCU Command: Simple Tool
# Sub Commnad : Save nand
echo $CAMCOMM simple_tool_save_nand >> $LOGF
RESULT=$($CAMCOMM simple_tool_save_nand)

# Change Camera Mode -------------------------------------------------
# IPCU Command: Change Camera Mode
# Sub Commnad : Change Mode
# Params      : UI指定値
echo $CAMCOMM chg_camera_mode $CAMERA_MODE >> $LOGF
RESULT=$($CAMCOMM chg_camera_mode $CAMERA_MODE)

# IPCU Command: Set Image Configuration
# Sub Commnad : ISO
# Params      : 0x00(Auto)
# TODO        : UIの値が設定されるんだけど、Autoにしたいわけじゃないの？？
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

# IPCU Command: Set Image Configuration
# Sub Commnad : White Balance (Mode)
# Params      : 0x00(Auto), 256(x1.0), 256(x1.0)
echo $CAMCOMM s_i_white_balance $WB_MODE 256 256 >> $LOGF
RESULT=$($CAMCOMM s_i_white_balance $WB_MODE 256 256)

$JO -p Result="Done."
