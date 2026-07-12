#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
read QUERY_STRING

CAMIF="camera_if"
TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# Data Expansion
M10V_IP=$(echo "$QUERY_STRING" | jq ".m10vipadd")
PC_XXX=$(echo "$QUERY_STRING" | jq ".pcmac")
PC_MAC=`echo $PC_XXX | sed -e 's/\"//g'`
PC_ZZZ=$(echo "$QUERY_STRING" | jq ".pcipadd")
PC_IP=`echo $PC_ZZZ | sed -e 's/\"//g'`

# "ifconfig"
#echo ifconfig mlan0 $M10V_IP >> $LOGF
#RESULT=$(ifconfig mlan0 $M10V_IP)

# wifi access
echo "gst-launch-1.0 -v ipcusrc ! h264parse ! rtph264pay ! udpsink force-ipv4=true host=$PC_IP port=5004 \&" >> $LOGF
RESULT=$(gst-launch-1.0 -v ipcusrc ! h264parse ! rtph264pay ! udpsink force-ipv4=true host=$PC_IP port=5004 \&)

# CAM_IF "finish_boot"
echo $CAMIF finish_boot >> $LOGF
RESULT=$($CAMIF finish_boot)

# idle mode
echo $CAMIF chg_camera_mode 1 >> $LOGF
RESULT=$($CAMIF chg_camera_mode 1)

# s_v_frame_size 
echo $CAMIF s_v_frame_size 4 5 1 >> $LOGF
RESULT=$($CAMIF s_v_frame_size 4 5 1)

# preview mode
echo $CAMIF chg_camera_mode 2 >> $LOGF
RESULT=$($CAMIF chg_camera_mode 2)

echo "v=0" > /var/www/M10Vp5004.sdp
echo "c=IN IP4 $PC_IP" >> /var/www/M10Vp5004.sdp
echo "m=video 5004 RTP/AVP 96" >> /var/www/M10Vp5004.sdp
echo "a=rtpmap:96 H264/90000" >> /var/www/M10Vp5004.sdp
echo "a=fmtp:96" >> /var/www/M10Vp5004.sdp

jo -p result="Done."
