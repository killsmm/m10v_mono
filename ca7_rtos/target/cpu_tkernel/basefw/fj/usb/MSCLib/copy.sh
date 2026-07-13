#! /bin/sh

proj_top_dir=`dirname "$0"`
cd "${proj_top_dir}"

. ./shell_subr.sh


cp -p lib/USB_Storage.a ../storagelib/lib

echo "> ls -l ../storagelib/lib"
ls -l ../storagelib/lib

ok_exit



