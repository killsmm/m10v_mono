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
echo /usr/bin/pmgr.sh get >> $LOGF
/usr/bin/pmgr.sh get

#RESPONSE 
RESULT=$?
echo GET RESULT=$RESULT  >> $LOGF

jo -p result="$RESULT"


