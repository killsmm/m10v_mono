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
SET_DATA=$(echo "$QUERY_STRING" | $JQ ".set_data")

# Read Camera Mode
CAMERA_MODE=$(echo "$QUERY_STRING" | $JQ ".camera_mode")
RESOLUTION=$(echo "$QUERY_STRING" | $JQ ".resolution")
ISO=$(echo "$QUERY_STRING" | $JQ ".iso")
WB_MODE=$(echo "$QUERY_STRING" | $JQ ".wb_mode")

# Read AE Table position
AE_TABLE_POS=$(echo "$QUERY_STRING" | $JQ ".ae_table_pos")

# Read IQ.bin's ID
IQ_BIN_INDEX=$(echo "$QUERY_STRING" | $JQ ".iq_bin_index")
AE_BIN_INDEX=$(echo "$QUERY_STRING" | $JQ ".ae_bin_index")

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

# Write To SDRAM -----------------------------------------------------
IQBINWR="membinrdwriq -w -m $IQ_BINX -o 216"
IQBINWRB="membinrdwriq -w -m $IQ_BINX -o 52"
AEBINWR0="membinrdwr -w -m $AE_COMM -o 40"
AEBINWR1="membinrdwr -w -m $AE_BINX -o 40"
AWBBINWR="membinrdwrawb -w -m $AWB_COMM -o 40"

# Write AE Table data
case $SET_TYPE in
	"\"AEMetering_Edit\"")
		SET_VALUE=$(echo "$SET_DATA" | jq ".ae_weight")
		echo $AEBINWR0 -i 2 -p $AE_TABLE_POS -n 1024 -f 2 \"$SET_VALUE\" >> $LOGF
		DEBUG_PRINT=$($AEBINWR0 -i 2 -p $AE_TABLE_POS -n 1024 -f 2 "$SET_VALUE")
		;;
esac

# Update module
UPDATE_MODE=-1

case $CAMERA_MODE in
	"1") 
		UPDATE_MODE=1
		;;
	"2")
		UPDATE_MODE=0
		;;
	"3")
		UPDATE_MODE=1
		;;
	"4")
		UPDATE_MODE=0
		;;
	"5")
		UPDATE_MODE=0
		;;
	"6")
		UPDATE_MODE=1
		;;
	*)
		UPDATE_MODE=-1
		;;
esac

case $UPDATE_MODE in
	"-1")
		;;
	*)
		# IPCU Command: Simple Tool
		# Sub Commnad : Update
		# Params      : Camera_Mode設定と対応する値
		echo $CAMCOMM simple_tool_update $UPDATE_MODE >> $LOGF
		# RESULT=$($CAMCOMM simple_tool_update $UPDATE_MODE)
		RESULT=$($CAMCOMM simple_tool_update 0)
		;;
esac

$JO -p Result="Done."