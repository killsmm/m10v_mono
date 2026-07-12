#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

#Param is none

# Parameter Manager
echo /usr/bin/pmgr.sh del >> $LOGF
/usr/bin/pmgr.sh del
RESULT=$?
echo DEL RESULT=$RESULT  >> $LOGF

#RESPONSE 
jo -p result="$RESULT"


