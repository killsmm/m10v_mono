#!/bin/sh

# ReciveDate format
echo Content-type: application/json; charset=UTF-8
echo 
# READ JSON Data
read QUERY_STRING

TIME=`date '+%y%m%d'`
LOGF="/tmp/simple_log_"${TIME}

# Data Expansion
HTMLVAL=$(echo "$QUERY_STRING" | jq ".jsval")
MENU_PARAM=`echo $HTMLVAL | sed -e 's/\"//g'`

# color 
#Gray	1
#Red	2
#Blue	3
#Yellow	4
#Green	5
#Cyan	6
#Magenta	7
#Whilte	8

case $MENU_PARAM in
 0 ) COLY=0 COLCB=0 COLCR=0;;
 1 ) COLY=126 COLCB=128 COLCR=128;;
 2 ) COLY=82 COLCB=90 COLCR=240;;
 3 ) COLY=41 COLCB=240 COLCR=110;;
 4 ) COLY=210 COLCB=16 COLCR=146;;
 5 ) COLY=145 COLCB=54 COLCR=34;;
 6 ) COLY=170 COLCB=166 COLCR=16;;
 7 ) COLY=107 COLCB=202 COLCR=222;;
 8 ) COLY=235 COLCB=128 COLCR=128;;
esac

#Set variable
#echo camera_if_direct 0x01010500 0x400 $COLY $COLCB $COLCR >> $LOGF
#RESULT=$(camera_if_direct 0x01010500 0x400  $COLY $COLCB $COLCR)
CMD="camera_if_direct 0x01010500 0x400 $COLY $COLCB $COLCR"
echo $CMD>> $LOGF
RESULT=$($CMD)

jo -p result="Done."
