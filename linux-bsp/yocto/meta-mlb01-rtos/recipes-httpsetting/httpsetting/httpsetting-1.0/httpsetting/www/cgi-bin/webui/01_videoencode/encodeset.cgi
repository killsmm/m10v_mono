#!/bin/sh

# クライアント側から受信したデータのフォーマット指定
echo Content-type: application/json; charset=UTF-8
echo 
# JSONデータ読み込み
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".ave")
AVE=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".max")
MAX=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".ch")
CH=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".gop")
GOP=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".alg")
ALG=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".sz")
SZ=`echo $TMP | sed -e 's/\"//g'`

#Bitrate
echo camera_if_direct 0x00010300 0x200 $CH $AVE $MAX $ALG >> $LOGF
RESULT=$(camera_if_direct 0x00010300 0x200 $CH $AVE $MAX $ALG)

#GOP
echo camera_if_direct 0x00010300 0x300 $CH $GOP >> $LOGF
RESULT=$(camera_if_direct 0x00010300 0x300 $CH $GOP)

#SIZE
echo camera_if_direct 0x00010300 0x600 $CH $SZ >> $LOGF
RESULT=$(camera_if_direct 0x00010300 0x600 $CH $SZ)

jo -p result="Done."

#READVAL=$(echo `camera_if_direct 0x80010300 0x200` | cut -c19-)
#P1=$(echo $READVAL | cut -c 12-21)
#P2=$(echo $READVAL | cut -c 34-43)
#P3=$(echo $READVAL | cut -c 56-65)
#P4=$(echo $READVAL | cut -c 78-87)
#
#echo camera_if_direct 0x10300 0x200 0x00 $AVE $VAL $P4 >> $LOGF
#RESULT=$(camera_if_direct 0x10300 0x200 0x00 $AVE $VAL $P4)

