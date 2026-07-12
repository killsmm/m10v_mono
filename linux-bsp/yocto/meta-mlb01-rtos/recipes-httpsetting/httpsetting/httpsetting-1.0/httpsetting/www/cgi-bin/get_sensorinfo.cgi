#!/bin/sh

echo Content-type: application/json; charset=UTF-8
echo 
read QUERY_STRING

JO="jo"
JQ="jq"
CAMCOMM="camera_cmd"
TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# IPCU Command: Get Acutual Shooting Condition
# Sub Commnad : Gain
# Return      : ISO(0～1600), Analog Gain(xxdB * 65536), Digital Gain(xxdB)
echo $CAMCOMM -r 3 g_as_gain >> $LOGF
GAIN=$($CAMCOMM -r 3 g_as_gain)

SENSOR_ISO=$(echo ${GAIN} | cut -d ',' -f 1)
SENSOR_A_GAIN=$(echo ${GAIN} | cut -d ',' -f 2)
SENSOR_D_GAIN=$(echo ${GAIN} | cut -d ',' -f 3)

# IPCU Command: Get Acutual Shooting Condition
# Sub Commnad : Exposure time
# Return      : Exposure time(xxsec * 66536)
echo $CAMCOMM -r 1 g_as_expo_time >> $LOGF
SENSOR_EXPO=$($CAMCOMM -r 1 g_as_expo_time)

# IPCU Command: Get Acutual Shooting Condition
# Sub Commnad : Color temperature
# Return      : Color temperature(xxxx * 65536)
echo $CAMCOMM -r 1 g_as_color_tempe >> $LOGF
SENSOR_COLORTEMP=$($CAMCOMM -r 1 g_as_color_tempe)

# IPCU Command: Get Acutual Shooting Condition
# Sub Commnad : White Balance
# Return      : R gain(0~2047), B gain(0~2047)
echo $CAMCOMM -r 4 g_as_white_balance >> $LOGF
WB=$($CAMCOMM -r 4 g_as_white_balance)

SENSOR_WB_R=$(echo ${WB} | cut -d ',' -f 1)
SENSOR_WB_B=$(echo ${WB} | cut -d ',' -f 4)

# Regist ack returns in json structure
JO_STATIC=$($JO statics=$($JO iso=$SENSOR_ISO a_gain=$SENSOR_A_GAIN d_gain=$SENSOR_D_GAIN expo=$SENSOR_EXPO c_temp=$SENSOR_COLORTEMP wb_r=$SENSOR_WB_R wb_b=$SENSOR_WB_B))
$JO -p Statics=$JO_STATIC


