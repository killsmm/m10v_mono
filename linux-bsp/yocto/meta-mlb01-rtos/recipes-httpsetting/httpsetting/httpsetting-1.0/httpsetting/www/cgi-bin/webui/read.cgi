#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

TMP=$(echo "$QUERY_STRING" | jq ".arg1")
ARG1=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg2")
ARG2=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg3")
ARG3=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg4")
ARG4=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg5")
ARG5=`echo $TMP | sed -e 's/\"//g'`
TMP=$(echo "$QUERY_STRING" | jq ".arg6")
ARG6=`echo $TMP | sed -e 's/\"//g'`

#echo ARG1=$ARG1 >> $LOGF
#echo ARG2=$ARG2 >> $LOGF
#echo ARG3=$ARG3 >> $LOGF
#echo ARG4=$ARG4 >> $LOGF
#echo ARG5=$ARG5 >> $LOGF
#echo ARG6=$ARG6 >> $LOGF

CAMIFSTR="camera_if_direct $ARG1 $ARG2 $ARG3 $ARG4 $ARG5 $ARG6"
echo $CAMIFSTR >> $LOGF
CAMIFRET=`$CAMIFSTR`
#echo $CAMIFRET  >> $LOGF

# Get Return Value.
oldifs=$IFS
IFS='
'
for i in $CAMIFRET; do
VAL=$i
if [ $(echo $VAL | grep -e 'Return 1') ]; then
RET_1=`echo $i | cut -c20-`
elif [ $(echo $VAL | grep -e 'Return 2') ]; then
RET_2=`echo $i | cut -c20-`
elif [ $(echo $VAL | grep -e 'Return 3') ]; then
RET_3=`echo $i | cut -c20-`
elif [ $(echo $VAL | grep -e 'Return 4') ]; then
RET_4=`echo $i | cut -c20-`
fi
done
IFS=$oldifs

#echo RET_1=$RET_1 >> $LOGF
#echo RET_2=$RET_2 >> $LOGF
#echo RET_3=$RET_3 >> $LOGF
#echo RET_4=$RET_4 >> $LOGF

jo -p ret1=$RET_1 ret2=$RET_2 ret3=$RET_3 ret4=$RET_4

