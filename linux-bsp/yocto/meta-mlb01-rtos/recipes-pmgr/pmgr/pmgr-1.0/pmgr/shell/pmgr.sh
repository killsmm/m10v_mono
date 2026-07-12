#!/bin/sh

PMGR_EXE=/usr/bin/pmgr
PMGR_CONF=/etc/pmgr/pmgr.conf

PMGR_NAND_DIR=/media/root/nand
PMGR_CSV_DIR=$PMGR_NAND_DIR/PMGR
DEFAULT_CSV=$PMGR_CSV_DIR/DEFAULT.CSV
PMGR_CSV=$PMGR_CSV_DIR/PMGR.CSV
ERRLOG_PATH=/dev/ttyUSI0 

PMGR_RESULT=0

PMGR_TMP=`mktemp`

trap "rm -f $PMGR_TMP" EXIT

if [ ! -d $PMGR_NAND_DIR ]; then
	mkdir $PMGR_NAND_DIR
	mount -t ipcufs mnt $PMGR_NAND_DIR -ofs=1
	PMGR_RESULT=$?
fi

if [ $PMGR_RESULT -eq 0 ]; then
	if [ ! -d $PMGR_CSV_DIR ]; then
		mkdir $PMGR_CSV_DIR
		PMGR_RESULT=$?
	fi
fi

if [ $PMGR_RESULT -ne 0 ]; then
	echo "NV area mount error. ($PMGR_NAND_DIR)" > $ERRLOG_PATH
	echo "Saved configuration could not be set to RTOS." > $ERRLOG_PATH
	exit $PMGR_RESULT
fi

case $1 in
set)
	if [ -e $DEFAULT_CSV ]; then
		if [ -e $PMGR_CSV ]; then
			$PMGR_EXE -l $PMGR_CSV
			PMGR_RESULT=$?
		fi
	else
		$PMGR_EXE -c $PMGR_CONF -s $PMGR_TMP
		PMGR_RESULT=$?
		if [ $PMGR_RESULT -eq 0 ]; then
			mv $PMGR_TMP $DEFAULT_CSV
			PMGR_RESULT=$?
		fi
	fi
	;;

get)
	if [ -e $DEFAULT_CSV ]; then
		$PMGR_EXE -c $PMGR_CONF -d $DEFAULT_CSV -s $PMGR_TMP
		PMGR_RESULT=$?
		if [ $PMGR_RESULT -eq 0 ]; then
			mv $PMGR_TMP $PMGR_CSV
			PMGR_RESULT=$?
		fi
	else
		$PMGR_EXE -c $PMGR_CONF -s $PMGR_TMP
		PMGR_RESULT=$?
		if [ $PMGR_RESULT -eq 0 ]; then
			mv $PMGR_TMP $DEFAULT_CSV
			PMGR_RESULT=$?
		fi
	fi
	;;

del)
	if [ -e $PMGR_CSV_DIR ]; then
		rm -rf $PMGR_CSV_DIR
		PMGR_RESULT=$?
	fi
	;;
esac

exit $PMGR_RESULT

