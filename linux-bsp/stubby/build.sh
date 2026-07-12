#!/bin/bash

for i in `ls configs/*.h` ; do
	echo $i
	make -C src -s CONFIG=$i
	if [ $? -ne 0 ] ; then
		exit 1
	fi
	BIN=`echo "$i" | sed "s|configs/config|stubby|g" | sed "s/\.h/.bin/g"`
	ls -l $BIN
done

