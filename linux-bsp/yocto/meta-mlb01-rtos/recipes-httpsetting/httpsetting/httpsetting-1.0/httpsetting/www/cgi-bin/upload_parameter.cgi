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

# Read IQ.bin's ID
IQ_BIN_INDEX=$(echo "$QUERY_STRING" | $JQ ".iq_bin_index")
AE_BIN_INDEX=$(echo "$QUERY_STRING" | $JQ ".ae_bin_index")

# Read ignore flag
IGNORE_UPDATE=$(echo "$QUERY_STRING" | $JQ ".ignore_update")

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
IQBINRDWR="membinrdwriq -w -m $IQ_BINX -o 216"
IQBINRDWRB="membinrdwriq -w -m $IQ_BINX -o 52"
AEBINRDWR0="membinrdwr -w -m $AE_COMM -o 40"
AEBINRDWR1="membinrdwr -w -m $AE_BINX -o 40"
AWBBINRDWR="membinrdwrawb -w -m $AWB_COMM -o 40"

echo "Upload type: ${SET_TYPE}" >> $LOGF

case $SET_TYPE in
	"\"IQ_gamma_index\"")
		# Gamma's BODY_SIZE = 12808
		# Update Index
		SET_VALUE=$(echo "$SET_DATA" | jq ".index")
		echo $IQBINRDWR -i 14 -n 1 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWR -i 14 -n 1 -a 0 "$SET_VALUE")
		;;
	"\"IQ_gamma\"")
		# Gamma's BODY_SIZE = 12808
		# Update Index
		SET_VALUE=$(echo "$SET_DATA" | jq ".index")
		echo $IQBINRDWR -i 14 -n 1 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWR -i 14 -n 1 -a 0 "$SET_VALUE")

		# Update Gamma index 0 --- Gamma Off(flat)
		SET_VALUE=$(echo "$SET_DATA" | jq ".data0.r_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 10760 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 10760 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data0.r_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 2056 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 2056 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data0.r_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 8 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 8 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data0.g_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 11272 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 11272 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data0.g_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 4104 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 4104 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data0.g_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 88 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 88 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data0.b_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 11784 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 11784 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data0.b_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 6152 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 6152 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data0.b_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 168 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 168 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data0.y_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 12296 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 12296 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data0.y_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 8200 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 8200 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data0.y_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 248 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 248 -n 40 -f 2 -a 0 "$SET_VALUE")

		# Update Gamma index 1 --- Pattern 1
		SET_VALUE=$(echo "$SET_DATA" | jq ".data1.r_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 23568 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 23568 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data1.r_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 14864 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 14864 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data1.r_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 12816 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 12816 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data1.g_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 24080 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 24080 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data1.g_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 16912 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 16912 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data1.g_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 12896 -n 40 -f 2 -a 0 "$SET_VALUE" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 12896 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data1.b_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 24592 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 24592 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data1.b_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 18960 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 18960 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data1.b_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 12976 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 12976 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data1.y_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 25104 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 25104 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data1.y_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 21008 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 21008 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data1.y_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 13056 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 13056 -n 40 -f 2 -a 0 "$SET_VALUE")

		# Update Gamma index 2 --- Pattern 2
		SET_VALUE=$(echo "$SET_DATA" | jq ".data2.r_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 36376 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 36376 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data2.r_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 27672 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 27672 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data2.r_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 25624 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 25624 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data2.g_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 36888 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 36888 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data2.g_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 29720 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 29720 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data2.g_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 25704 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 25704 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data2.b_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 37400 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 37400 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data2.b_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 31768 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 31768 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data2.b_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 25784 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 25784 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data2.y_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 37912 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 37912 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data2.y_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 33816 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 33816 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data2.y_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 25864 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 25864 -n 40 -f 2 -a 0 "$SET_VALUE")

		# Update Gamma index 3 --- Pattern 3
		SET_VALUE=$(echo "$SET_DATA" | jq ".data3.r_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 49184 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 49184 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data3.r_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 40480 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 40480 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data3.r_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 38432 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 38432 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data3.g_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 49696 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 49696 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data3.g_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 42528 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 42528 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data3.g_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 38512 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 38512 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data3.b_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 50208 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 50208 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data3.b_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 44576 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 44576 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data3.b_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 38592 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 38592 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data3.y_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 50720 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 50720 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data3.y_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 46624 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 46624 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data3.y_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 38672 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 38672 -n 40 -f 2 -a 0 "$SET_VALUE")

		# Update Gamma index 4 --- Pattern 4
		SET_VALUE=$(echo "$SET_DATA" | jq ".data4.r_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 61992 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 61992 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data4.r_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 53288 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 53288 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data4.r_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 51240 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 51240 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data4.g_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 62504 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 62504 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data4.g_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 55336 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 55336 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data4.g_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 51320 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 51320 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data4.b_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 63016 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 63016 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data4.b_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 57384 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 57384 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data4.b_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 51400 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 51400 -n 40 -f 2 -a 0 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".data4.y_full")
		echo $IQBINRDWRB -i 13 -b 0 -p 63528 -n 256 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 63528 -n 256 -f 2 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data4.y_diff")
		echo $IQBINRDWRB -i 13 -b 0 -p 59432 -n 512 -f 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 59432 -n 512 -f 4 -a 0 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".data4.y_opepos")
		echo $IQBINRDWRB -i 13 -b 0 -p 51480 -n 40 -f 2 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 13 -b 0 -p 51480 -n 40 -f 2 -a 0 "$SET_VALUE")
		;;
	"\"IQ_sharpness\"")
		SET_VALUE=$(echo "$SET_DATA" | jq ".sharpness")
		echo $IQBINRDWR -i 6 -n 7 -a 0  \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWR -i 6 -n 7 -a 0  "$SET_VALUE")
		SET_VALUE=$(echo "$SET_DATA" | jq ".edge")
		echo $IQBINRDWR -i 20 -n 7 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWR -i 20 -n 7 -a 0 "$SET_VALUE")
		;;
	"\"IQ_denoise\"")
		SET_VALUE=$(echo "$SET_DATA" | jq ".nr_y")
		echo $IQBINRDWR -i 3 -n 7 -a 15 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWR -i 3 -n 7 -a 15 "$SET_VALUE")
		SET_VALUE=$(echo "$SET_DATA" | jq ".nr_c")
		echo $IQBINRDWR -i 32 -n 7 -a 15 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWR -i 32 -n 7 -a 15 "$SET_VALUE")
		;;
	"\"IQ_nr3d\"")
		SET_VALUE=$(echo "$SET_DATA" | jq ".nr")
		echo $IQBINRDWR -i 36 -n 7 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWR -i 36 -n 7 -a 0 "$SET_VALUE")
		;;
	"\"IQ_sensor\"")
		# Update OB value's ISO100 index
		SET_VALUE=$(echo "$SET_DATA" | jq ".data[0]")
		echo $IQBINRDWRB -i 1 -b 0 -p 0 -f 2 -n 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 1 -b 0 -p 0 -f 2 -n 4 -a 0 "$SET_VALUE")
		
		# Update OB value's ISO200 index
		SET_VALUE=$(echo "$SET_DATA" | jq ".data[1]")
		echo $IQBINRDWRB -i 1 -b 0 -p 8 -f 2 -n 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 1 -b 0 -p 8 -f 2 -n 4 -a 0 "$SET_VALUE")
		
		# Update OB value's ISO400 index
		SET_VALUE=$(echo "$SET_DATA" | jq ".data[2]")
		echo $IQBINRDWRB -i 1 -b 0 -p 16 -f 2 -n 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 1 -b 0 -p 16 -f 2 -n 4 -a 0 "$SET_VALUE")
		
		# Update OB value's ISO800 index
		SET_VALUE=$(echo "$SET_DATA" | jq ".data[3]")
		echo $IQBINRDWRB -i 1 -b 0 -p 24 -f 2 -n 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 1 -b 0 -p 24 -f 2 -n 4 -a 0 "$SET_VALUE")
		
		# Update OB value's ISO1600 index
		SET_VALUE=$(echo "$SET_DATA" | jq ".data[4]")
		echo $IQBINRDWRB -i 1 -b 0 -p 32 -f 2 -n 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 1 -b 0 -p 32 -f 2 -n 4 -a 0 "$SET_VALUE")
		
		# Update OB value's ISO3200 index
		SET_VALUE=$(echo "$SET_DATA" | jq ".data[5]")
		echo $IQBINRDWRB -i 1 -b 0 -p 40 -f 2 -n 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 1 -b 0 -p 40 -f 2 -n 4 -a 0 "$SET_VALUE")
		
		# Update OB value's ISO6400 index
		SET_VALUE=$(echo "$SET_DATA" | jq ".data[6]")
		echo $IQBINRDWRB -i 1 -b 0 -p 48 -f 2 -n 4 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWRB -i 1 -b 0 -p 48 -f 2 -n 4 -a 0 "$SET_VALUE")
		;;
	"\"IQ_color\"")
		SET_VALUE=$(echo "$SET_DATA" | jq ".saturation")
		echo $IQBINRDWR -i 28 -n 7 -a 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($IQBINRDWR -i 28 -n 7 -a 0 "$SET_VALUE")
		;;
	"\"AE_common\"")
		SET_VALUE=$(echo "$SET_DATA" | jq ".f_number")
		echo $AEBINRDWR0 -i 0 -p 0 -f 64 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 0 -p 0 -f 64 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog0")
		echo $AEBINRDWR0 -i 1 -p 96 -s 32 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 96 -s 32 -n 8 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".prog1")
		echo $AEBINRDWR0 -i 1 -p 288 -s 36 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 288 -s 36 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog2")
		echo $AEBINRDWR0 -i 1 -p 480 -s 40 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 480 -s 40 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog3")
		echo $AEBINRDWR0 -i 1 -p 672 -s 44 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 672 -s 44 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog4")
		echo $AEBINRDWR0 -i 1 -p 864 -s 48 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 864 -s 48 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog5")
		echo $AEBINRDWR0 -i 1 -p 1056 -s 52 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 1056 -s 52 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog6")
		echo $AEBINRDWR0 -i 1 -p 1248 -s 56 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 1248 -s 56 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog7")
		echo $AEBINRDWR0 -i 1 -p 1440 -s 60 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 1440 -s 60 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog8")
		echo $AEBINRDWR0 -i 1 -p 1632 -s 64 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 1632 -s 64 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog9")
		echo $AEBINRDWR0 -i 1 -p 1824 -s 68 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 1824 -s 68 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog10")
		echo $AEBINRDWR0 -i 1 -p 2016 -s 72 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 2016 -s 72 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog11")
		echo $AEBINRDWR0 -i 1 -p 2208 -s 76 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 2208 -s 76 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog12")
		echo $AEBINRDWR0 -i 1 -p 2400 -s 80 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 2400 -s 80 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog13")
		echo $AEBINRDWR0 -i 1 -p 2592 -s 84 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 2592 -s 84 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog14")
		echo $AEBINRDWR0 -i 1 -p 2784 -s 88 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 2784 -s 88 -n 8 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog15")
		echo $AEBINRDWR0 -i 1 -p 2976 -s 92 -n 8 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 2976 -s 92 -n 8 "$SET_VALUE")
		;;
	"\"AE_evprogram\"")
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog0")
		echo $AEBINRDWR0 -i 1 -p 96 -s 32 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 96 -s 32 -n 4 "$SET_VALUE")

		SET_VALUE=$(echo "$SET_DATA" | jq ".prog1")
		echo $AEBINRDWR0 -i 1 -p 288 -s 36 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 288 -s 36 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog2")
		echo $AEBINRDWR0 -i 1 -p 480 -s 40 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 480 -s 40 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog3")
		echo $AEBINRDWR0 -i 1 -p 672 -s 44 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 672 -s 44 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog4")
		echo $AEBINRDWR0 -i 1 -p 864 -s 48 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 864 -s 48 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog5")
		echo $AEBINRDWR0 -i 1 -p 1056 -s 52 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 1056 -s 52 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog6")
		echo $AEBINRDWR0 -i 1 -p 1248 -s 56 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 1248 -s 56 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog7")
		echo $AEBINRDWR0 -i 1 -p 1440 -s 60 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 1440 -s 60 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog8")
		echo $AEBINRDWR0 -i 1 -p 1632 -s 64 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 1632 -s 64 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog9")
		echo $AEBINRDWR0 -i 1 -p 1824 -s 68 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 1824 -s 68 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog10")
		echo $AEBINRDWR0 -i 1 -p 2016 -s 72 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 2016 -s 72 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog11")
		echo $AEBINRDWR0 -i 1 -p 2208 -s 76 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 2208 -s 76 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog12")
		echo $AEBINRDWR0 -i 1 -p 2400 -s 80 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 2400 -s 80 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog13")
		echo $AEBINRDWR0 -i 1 -p 2592 -s 84 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 2592 -s 84 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog14")
		echo $AEBINRDWR0 -i 1 -p 2784 -s 88 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 2784 -s 88 -n 4 "$SET_VALUE")
		
		SET_VALUE=$(echo "$SET_DATA" | jq ".prog15")
		echo $AEBINRDWR0 -i 1 -p 2976 -s 92 -n 4 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AEBINRDWR0 -i 1 -p 2976 -s 92 -n 4 "$SET_VALUE")
		;;
	"\"AE_default\"")
		AE_DEFAULTSET_AE_METERING=$(echo "$SET_DATA" | jq ".ae_metering")
		echo $AEBINRDWR1 -i 1 -p 0 \"$AE_DEFAULTSET_AE_METERING\" >> $LOGF
		RESULT=$($AEBINRDWR1 -i 1 -p 0 "$AE_DEFAULTSET_AE_METERING")
		
		AE_DEFAULTSET_AE_MODE=$(echo "$SET_DATA" | jq ".ae_mode")
		echo $AEBINRDWR1 -i 1 -p 4 \"$AE_DEFAULTSET_AE_MODE\" >> $LOGF
		RESULT=$($AEBINRDWR1 -i 1 -p 4 "$AE_DEFAULTSET_AE_MODE")
		
		AE_DEFAULTSET_AE_SPEED=$(echo "$SET_DATA" | jq ".ae_speed")
		echo $AEBINRDWR1 -i 1 -p 8 \"$AE_DEFAULTSET_AE_SPEED\" >> $LOGF
		RESULT=$($AEBINRDWR1 -i 1 -p 8 "$AE_DEFAULTSET_AE_SPEED")
		
		AE_DEFAULTSET_AE_ADAPTIVE=$(echo "$SET_DATA" | jq ".ae_adaptive")
		echo $AEBINRDWR1 -i 1 -p 12 \"$AE_DEFAULTSET_AE_ADAPTIVE\" >> $LOGF
		RESULT=$($AEBINRDWR1 -i 1 -p 12 "$AE_DEFAULTSET_AE_ADAPTIVE")

		AE_DEFAULTSET_EVPROGRAM=$(echo "$SET_DATA" | jq ".evprogram")
		echo $AEBINRDWR1 -i 1 -p 16 \"$AE_DEFAULTSET_EVPROGRAM\" >> $LOGF
		RESULT=$($AEBINRDWR1 -i 1 -p 16 "$AE_DEFAULTSET_EVPROGRAM")

		AE_DEFAULTSET_FLICKER_COMP=$(echo "$SET_DATA" | jq ".flicker_comp")
		echo $AEBINRDWR1 -i 1 -p 24 \"$AE_DEFAULTSET_FLICKER_COMP\" >> $LOGF
		RESULT=$($AEBINRDWR1 -i 1 -p 24 "$AE_DEFAULTSET_FLICKER_COMP")

		AE_DEFAULTSET_EV_COMP=$(echo "$SET_DATA" | jq ".ev_comp")
		echo $AEBINRDWR1 -i 1 -p 20 \"$AE_DEFAULTSET_EV_COMP\" >> $LOGF
		RESULT=$($AEBINRDWR1 -i 1 -p 20 "$AE_DEFAULTSET_EV_COMP")

		AE_DEFAULTSET_SENSOR_GAIN=$(echo "$SET_DATA" | jq ".sensor_gain")
		echo $AEBINRDWR1 -i 1 -p 28 \"$AE_DEFAULTSET_SENSOR_GAIN\" >> $LOGF
		RESULT=$($AEBINRDWR1 -i 1 -p 28 "$AE_DEFAULTSET_SENSOR_GAIN")

		AE_DEFAULTSET_SENSOR_EXPOSURE=$(echo "$SET_DATA" | jq ".sensor_exposure")
		echo $AEBINRDWR1 -i 1 -p 32 -f 64 \"$AE_DEFAULTSET_SENSOR_EXPOSURE\" >> $LOGF
		RESULT=$($AEBINRDWR1 -i 1 -p 32 -f 64 "$AE_DEFAULTSET_SENSOR_EXPOSURE")

		# Reflect AESettings
		echo sh ../reflect_aesettings.sh $AE_DEFAULTSET_AE_METERING $AE_DEFAULTSET_AE_MODE $AE_DEFAULTSET_AE_SPEED $AE_DEFAULTSET_AE_ADAPTIVE $AE_DEFAULTSET_EVPROGRAM $AE_DEFAULTSET_FLICKER_COMP $AE_DEFAULTSET_EV_COMP $AE_DEFAULTSET_SENSOR_GAIN $AE_DEFAULTSET_SENSOR_EXPOSURE >> $LOGF
		RESULT=$(sh ../reflect_aesettings.sh $AE_DEFAULTSET_AE_METERING $AE_DEFAULTSET_AE_MODE $AE_DEFAULTSET_AE_SPEED $AE_DEFAULTSET_AE_ADAPTIVE $AE_DEFAULTSET_EVPROGRAM $AE_DEFAULTSET_FLICKER_COMP $AE_DEFAULTSET_EV_COMP $AE_DEFAULTSET_SENSOR_GAIN $AE_DEFAULTSET_SENSOR_EXPOSURE)
		;;
	"\"AWB_basic\"")
		SET_VALUE=$(echo "$SET_DATA" | jq ".awb_speed")
		echo $AWBBINRDWR -i 0 -p 24 -f 1 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AWBBINRDWR -i 0 -p 24 -f 1 "$SET_VALUE")
		;;
	"\"AWB_preferedcast\"")
		SET_VALUE=$(echo "$SET_DATA")
		echo $AWBBINRDWR -i 1 -p 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AWBBINRDWR -i 1 -p 0 "$SET_VALUE")
		;;
	"\"AWB_manualwb\"")
		SET_VALUE=$(echo "$SET_DATA")
		echo $AWBBINRDWR -i 2 -p 0 \"$SET_VALUE\" >> $LOGF
		RESULT=$($AWBBINRDWR -i 2 -p 0 "$SET_VALUE")
		;;
	*)
		echo "Unknown contents: ${SET_DATA}" >> $LOGF
		;;
esac

case $IGNORE_UPDATE in
	"\"true\"")
		;;
	*)
		# Update module
		# IPCU Command: Simple Tool
		# Sub Commnad : Update
		# Params      : video update
		echo $CAMCOMM simple_tool_update 0 >> $LOGF
		RESULT=$($CAMCOMM simple_tool_update 0)

#		case $CAMERA_MODE in
#			"3")
#				# Update module
#				# IPCU Command: Simple Tool
#				# Sub Commnad : Update
#				# Params      : photo update
#				echo $CAMCOMM simple_tool_update 1 >> $LOGF
#				RESULT=$($CAMCOMM simple_tool_update 1)
#				;;
#			*)
#				;;
#		esac
		;;
esac

$JO -p Result="Done."