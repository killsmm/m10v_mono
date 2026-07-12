#!/bin/sh

# XCS0 NOR (16-bit)
MTDNAME="physmap-flash.1 11000000.flash"

MTD=
for i in `mtdinfo|grep Present|tr ',' ' '|tr -s ' ' | cut -d ' ' -f4-` ; do
 for j in $MTDNAME ; do
  if [ ! -z "`mtdinfo /dev/$i 2>/dev/null | grep $j`" ] ; then
   MTD=/dev/$i
  fi
 done
done

if [ -z "$MTD" ] ; then
 echo "Cannot find $MTDNAME"
 exit 1
fi

flash_erase $MTD 0 1 && \
dd if=stubby.bin of=$MTD

