#!/bin/bash

CURNTDIR=`pwd`
STUBBY_MAX_SIZE=32768
ROMFS_ENTRY_LEN=0x20
ROMFS_IN_DDR=0x41000000
STUBBYDIR=$CURNTDIR/stubby
KERNELDIR=$CURNTDIR/kernel
INIRAMDIR=$CURNTDIR/initram

CheckExist() { # file OKMsg ErrorMsg
  if [ -e $1 ]; then
    echo "$2"
  else
    echo "Error: $3"
    exit 1
  fi
}

if [ "$1" = "checkSkip" ]; then
  CheckExist $STUBBYDIR/stubby.bin "" "Not copy $CURNTDIR/stubby.bin"
else
  echo "[File check]"
  CheckExist $STUBBYDIR/stubby.bin "stubby.bin OK" "stubby.bin NG"
  CheckExist $KERNELDIR/boot/Image-nolpae "zImeage OK" "zImeage NG"
  CheckExist $KERNELDIR/boot/sc2000.dtb "DTB OK" "DTB NG"
  CheckExist $INIRAMDIR/initramfs "initramfs OK" "initramfs NG"
fi

echo "make romfs start"
cp $STUBBYDIR/stubby.bin _stubby_romfs
genromfs -v -A 4096,Image-nolpae -d "$KERNELDIR" -f _kernel_romfs 2> _k_map
genromfs -v -d "$INIRAMDIR" -f _initram 2> _i_map

mkdir -p _output

STUBBY_SIZE=`ls -l _stubby_romfs | awk '{print $5}'`
head -c $((STUBBY_MAX_SIZE - STUBBY_SIZE)) /dev/zero >  alignment.bin

cat _stubby_romfs alignment.bin _kernel_romfs > _output/LINUX.BIN
cp _initram _output/initramfs

# cleanup
rm -f _stubby_romfs alignment.bin _kernel_romfs _initram _*_map
echo "make romfs end"

