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
# Read Contents Type
SET_TYPE=$(echo "$QUERY_STRING" | $JQ ".set_type")
CAMERA_MODE=$(echo "$QUERY_STRING" | $JQ ".camera_mode")
RESOLUTION=$(echo "$QUERY_STRING" | $JQ ".resolution")
ISO=$(echo "$QUERY_STRING" | $JQ ".iso")
WB_MODE=$(echo "$QUERY_STRING" | $JQ ".wb_mode")

# if Capturing is done.
case $SET_TYPE in
	"\"Start\"")
                # Set up capture
                echo sh ../v_cap_setup.sh $CAMERA_MODE $RESOLUTION $ISO $WB_MODE >> $LOGF
                RESULT=$(sh ../v_cap_setup.sh $CAMERA_MODE $RESOLUTION $ISO $WB_MODE)

                # Change Camera Mode -------------------------------------------------
                # IPCU Command: Change Camera Mode
                # Sub Commnad : Change Mode
                # Params      : Video Capture
                echo $CAMCOMM chg_camera_mode 8 >> $LOGF
                RESULT=$($CAMCOMM chg_camera_mode 8)
                ;;
	"\"Stop\"")
                # Finalyze capture
                echo sh ../v_cap_finalyze.sh $CAMERA_MODE $RESOLUTION $ISO $WB_MODE >> $LOGF
                RESULT=$(sh ../v_cap_finalyze.sh $CAMERA_MODE $RESOLUTION $ISO $WB_MODE)

                # Change Camera Mode -------------------------------------------------
                # IPCU Command: Change Camera Mode
                # Sub Commnad : Change Mode
                # Params      : Still Capture
                echo $CAMCOMM chg_camera_mode 2 >> $LOGF
                RESULT=$($CAMCOMM chg_camera_mode 2)

                # IPCU Command: Set Image Configuration
                # Sub Commnad : ISO
                # Params      : 0x00(Auto)
                # TODO        : UIの値が設定されるんだけど、Autoにしたいわけじゃないの？？
                echo $CAMCOMM s_i_iso $ISO >> $LOGF
                RESULT=$($CAMCOMM s_i_iso $ISO)

                # Update module
                # IPCU Command: Simple Tool
                # Sub Commnad : Update
                # Params      : video update
                echo $CAMCOMM simple_tool_update 0 >> $LOGF
                RESULT=$($CAMCOMM simple_tool_update 0)

                # IPCU Command: Set Image Configuration
                # Sub Commnad : White Balance (Mode)
                # Params      : 0x00(Auto), 256(x1.0), 256(x1.0)
                echo $CAMCOMM s_i_white_balance $WB_MODE 256 256 >> $LOGF
                RESULT=$($CAMCOMM s_i_white_balance $WB_MODE 256 256)
                ;;
	*)
		;;
esac

$JO -p Result="Done."
