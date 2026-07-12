#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
read QUERY_STRING

CAMIF="camera_if"
TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# Data Expansion
M10V_YYY=$(echo "$QUERY_STRING" | jq ".m10vipadd")
M10V_IP=`echo $M10V_YYY | sed -e 's/\"//g'`
PC_XXX=$(echo "$QUERY_STRING" | jq ".pcmac")
PC_MAC=`echo $PC_XXX | sed -e 's/\"//g'`
PC_ZZZ=$(echo "$QUERY_STRING" | jq ".pcipadd")
PC_IP=`echo $PC_ZZZ | sed -e 's/\"//g'`

# for OSD initialize
CMD="osdcmd 0,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 1,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 2,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 3,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 4,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 5,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 6,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 7,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 8,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 9,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 10,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 11,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 12,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 13,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 14,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 15,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 16,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 17,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 18,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)
CMD="osdcmd 19,2147483647"
echo $CMD >> $LOGF
RESULT=$($CMD)

ps | grep "osdcmd 2" | grep -v grep
if [ $? == 1 ]; then
CMD="osdcmd 2,3,0,0,0,1,0,\" \" &"
echo $CMD >> $LOGF
RESULT=$($CMD)
fi
ps | grep "osdcmd 5" | grep -v grep
if [ $? == 1 ]; then
CMD="osdcmd 5,3,0,0,0,1,0,\" \" &"
echo $CMD >> $LOGF
RESULT=$($CMD)
fi
ps | grep "osdcmd 8" | grep -v grep
if [ $? == 1 ]; then
CMD="osdcmd 8,3,0,0,0,1,0,\" \" &"
echo $CMD >> $LOGF
RESULT=$($CMD)
fi
ps | grep "osdcmd 11" | grep -v grep
if [ $? == 1 ]; then
CMD="osdcmd 11,3,0,0,0,1,0,\" \" &"
echo $CMD >> $LOGF
RESULT=$($CMD)
fi

# "ifconfig"
#echo ifconfig eth0 $M10V_IP >> $LOGF
#RESULT=$(ifconfig eth0 $M10V_IP)
# ether access
ps | grep netsecrtp | grep -v grep
if [ $? == 1 ]; then
CMD="streamd $PC_IP $PC_MAC"
echo $CMD >> $LOGF
RESULT=$($CMD)
# Notice:Error return does not return soon.
fi

jo -p result="Done."
