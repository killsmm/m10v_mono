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
RESOLUTION=$(echo "$QUERY_STRING" | $JQ ".resolution")

# Read IQ.bin's ID
IQ_BIN_INDEX=$(echo "$QUERY_STRING" | $JQ ".iq_bin_index")
AE_BIN_INDEX=$(echo "$QUERY_STRING" | $JQ ".ae_bin_index")

# if Capturing is done.
case $CAMERA_MODE in
	"2") 
                # Change Camera Mode -------------------------------------------------
                # IPCU Command: Change Camera Mode
                # Sub Commnad : Change Mode
                # Params      : Idle
                echo $CAMCOMM chg_camera_mode 1 >> $LOGF
                RESULT=$($CAMCOMM chg_camera_mode 1)

                # Update resolution
                echo sh ../set_resolution.sh $CAMERA_MODE $RESOLUTION >> $LOGF
                RESULT=$(sh ../set_resolution.sh $CAMERA_MODE $RESOLUTION)
                
                # Change Camera Mode -------------------------------------------------
                # IPCU Command: Change Camera Mode
                # Sub Commnad : Change Mode
                # Params      : Still Capture
                echo $CAMCOMM chg_camera_mode 2 >> $LOGF
                RESULT=$($CAMCOMM chg_camera_mode 2)
                ;;
        *)
                ;;
esac

# Read and localize IQ.bin address -----------------------------------
# IPCU Command: Simple Tool
# Sub Commnad : Get Address
# Params      : 0x00(IQ), ID
# Return      : Address
echo $CAMCOMM -r 2 simple_tool_get_addr 0x0 $IQ_BIN_INDEX >> $LOGF
IQ_BINX=$($CAMCOMM -r 2 simple_tool_get_addr 0x0 $IQ_BIN_INDEX)

# IPCU Command: Simple Tool
# Sub Commnad : Get Address
# Params      : 0x01(AE), ID
# Return      : Common's Address
echo $CAMCOMM -r 2 simple_tool_get_addr 0x1 0x0 >> $LOGF
AE_COMM=$($CAMCOMM -r 2 simple_tool_get_addr 0x1 0x0)

# IPCU Command: Simple Tool
# Sub Commnad : Get Address
# Params      : 0x01(AE), ID
# Return      : Address
echo $CAMCOMM -r 2 simple_tool_get_addr 0x1 $AE_BIN_INDEX >> $LOGF
AE_BINX=$($CAMCOMM -r 2 simple_tool_get_addr 0x1 $AE_BIN_INDEX)

# IPCU Command: Simple Tool
# Sub Commnad : Get Address
# Params      : 0x02(AWB), ID
# Return      : Common's Address
echo $CAMCOMM -r 2 simple_tool_get_addr 0x2 0x0 >> $LOGF
AWB_COMM=$($CAMCOMM -r 2 simple_tool_get_addr 0x2 0x0)

# Read SDRAM data ----------------------------------------------------
IQBINRD="membinrdwriq -r -m $IQ_BINX -o 216"
IQBINRDB="membinrdwriq -r -m $IQ_BINX -o 52"
AEBINRD0="membinrdwr -r -m $AE_COMM -o 40"
AEBINRD1="membinrdwr -r -m $AE_BINX -o 40"
AWBBINRD="membinrdwrawb -r -m $AWB_COMM -o 40"

# Read AE
AE_DEFAULTSET_AE_METERING=$($AEBINRD1 -i 1 -p 0)
AE_DEFAULTSET_AE_MODE=$($AEBINRD1 -i 1 -p 4)
AE_DEFAULTSET_AE_SPEED=$($AEBINRD1 -i 1 -p 8)
AE_DEFAULTSET_AE_ADAPTIVE=$($AEBINRD1 -i 1 -p 12)
AE_DEFAULTSET_EVPROGRAM=$($AEBINRD1 -i 1 -p 16)
AE_DEFAULTSET_FLICKER_COMP=$($AEBINRD1 -i 1 -p 24)
AE_DEFAULTSET_EV_COMP=$($AEBINRD1 -i 1 -p 20)
AE_DEFAULTSET_SENSOR_GAIN=$($AEBINRD1 -i 1 -p 28)
AE_DEFAULTSET_SENSOR_EXPOSURE=$($AEBINRD1 -i 1 -p 32 -f 64)

AE_DEFSET=$($JO ae_metering=$AE_DEFAULTSET_AE_METERING ae_mode=$AE_DEFAULTSET_AE_MODE ae_speed=$AE_DEFAULTSET_AE_SPEED ae_adaptive=$AE_DEFAULTSET_AE_ADAPTIVE evprogram=$AE_DEFAULTSET_EVPROGRAM flicker_comp=$AE_DEFAULTSET_FLICKER_COMP ev_comp=$AE_DEFAULTSET_EV_COMP sensor_gain=$AE_DEFAULTSET_SENSOR_GAIN sensor_exposure=$AE_DEFAULTSET_SENSOR_EXPOSURE)
JO_AE=$($JO defaultset=$AE_DEFSET)

$JO -p AE="$JO_AE"

