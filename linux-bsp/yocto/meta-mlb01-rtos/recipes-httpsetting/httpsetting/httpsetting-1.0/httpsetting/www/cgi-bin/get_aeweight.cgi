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

# Read SDRAM data ----------------------------------------------------
IQBINRD="membinrdwriq -r -m $IQ_BINX -o 216"
IQBINRDB="membinrdwriq -r -m $IQ_BINX -o 52"
AEBINRD0="membinrdwr -r -m $AE_COMM -o 40"
AEBINRD1="membinrdwr -r -m $AE_BINX -o 40"
AWBBINRD="membinrdwrawb -r -m $AWB_COMM -o 40"

# Read AE Table
echo $AEBINRD0 -i 2 -p $AE_TABLE_POS -n 1024 -f 2 >> $LOGF
AE_WEIGHT_TABLE=$($AEBINRD0 -i 2 -p $AE_TABLE_POS -n 1024 -f 2)

# Regist ack returns in json structure
JO_AE=$($JO ae_weight="$AE_WEIGHT_TABLE")
$JO -p AE="$JO_AE"


