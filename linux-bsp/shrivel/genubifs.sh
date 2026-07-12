#!/bin/sh

if [ -z "$1" -o -z "$2" -o ! -d $1 ] ; then
	echo "Usage: $0 <source dir>  <ubifs output name>"
	exit 1
fi

M=2048
E=126976
C=3600

sudo mkfs.ubifs \
	-r $1 \
	-o $2 \
	-m $M -e $E -c $C

