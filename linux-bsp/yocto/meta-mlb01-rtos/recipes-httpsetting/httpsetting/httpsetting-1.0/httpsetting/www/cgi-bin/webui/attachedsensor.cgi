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

# CAM_IF "finish_boot"
CMD="camera_if finish_boot"
echo $CMD >> $LOGF
RESULT=$($CMD)

# idle mode
CMD="camera_if chg_camera_mode 1"
echo $CMD >> $LOGF
RESULT=$($CMD)

four_sensor_init () {
	CMD="camera_if_direct 0x00010300 0x00000100 0x0 0x0 0x7 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000100 0x0 0x2 0xc 0x2"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x01030100 0x00000100 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010900 0x00000400 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010900 0x00000300 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000200 0x0 0xd59f80 0xe4e1c0 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000300 0x0 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x0 0x4"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x1 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x2 0x5"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x3 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
}

dual_sensor_init () {
	CMD="camera_if_direct 0x00010300 0x00000100 0x1 0x0 0x5 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000100 0x1 0x1 0x5 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000100 0x1 0x2 0x5 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000100 0x1 0x3 0x5 0x2"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x01030100 0x00000200 0x3"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010900 0x00000400 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010900 0x00000300 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000200 0x0 0xd59f80 0xe4e1c0 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000200 0x1 0x6acfc0 0x7a1200 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000200 0x2 0x4c4b40 0x5b8d80 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000300 0x0 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000300 0x1 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000300 0x2 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x0 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x1 0x4"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x2 0x5"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x3 0x7"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
}

single_dol8M_sensor_init () {
	CMD="camera_if_direct 0x00010300 0x00000100 0x2 0x0 0x2 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000100 0x2 0x1 0x2 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000100 0x2 0x2 0x5 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x01030200 0x00000100 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010900 0x00000400 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010900 0x00000300 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000200 0x0 0xd59f80 0xe4e1c0 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000200 0x1 0xd59f80 0xe4e1c0 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000200 0x2 0x2dc6c0 0x3d0900 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000300 0x0 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000300 0x1 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000300 0x2 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x0 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x1 0x4"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x2 0x7"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x3 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
}

single_dol5M_sensor_init () {
	CMD="camera_if_direct 0x00010300 0x00000100 0x3 0x0 0x9 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000100 0x3 0x1 0xe 0x2"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000100 0x3 0x2 0xe 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x01030200 0x00000100 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010900 0x00000400 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010900 0x00000300 0x1"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000200 0x0 0xd59f80 0xe4e1c0 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000200 0x2 0x6acfc0 0x7a1200 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000300 0x0 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000300 0x2 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x0 0x2"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x1 0x5"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x2 0x9"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
	CMD="camera_if_direct 0x00010300 0x00000600 0x3 0x0"
	echo $CMD >> $LOGF
	RESULT=$($CMD)
}


if [ $MENU_PARAM -eq 0 ] ; then
 echo "success else"
elif [ $MENU_PARAM -eq 4 ] ; then
	four_sensor_init
elif [ $MENU_PARAM -eq 2 ] ; then
	dual_sensor_init
elif [ $MENU_PARAM -eq 3 ] ; then
	single_dol8M_sensor_init
elif [ $MENU_PARAM -eq 1 ] ; then
	single_dol5M_sensor_init
else
 echo "success else"
fi


CMD="camera_if chg_camera_mode 2"
echo $CMD >> $LOGF
RESULT=$($CMD)

jo -p result="Done."
