#!/bin/bash

mkdir -p tmp_stage

cd tmp_stage

rm *

cp ../make_BIN_linux/2GB/initramfs_2core ./

tar cf update.tar *

checksum=($(md5sum update.tar))

echo $checksum

mv update.tar "$checksum"_update.tar
